#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QMessageBox>
#include <QSignalBlocker>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentPlayer(1)
    , scoreBoardDisabled(false)
    , gameStarted(false)
    , tempWin1(0) // افزودن این خطوط
    , tempLose1(0)
    , tempDraw1(0)
    , tempWin2(0)
    , tempLose2(0)
    , tempDraw2(0)

{
    ui->setupUi(this);

    // غیرفعال کردن تمام دکمه‌ها و گزینه‌های بازی در ابتدا
    disableAllButtons();
    ui->gridLayoutWidget_4->setVisible(false); // مخفی کردن گزینه‌های موافقت/ریست
    ui->lblGameOver->setVisible(false);        // مخفی کردن برچسب بازی تمام شد
    ui->lblWinnerTitle->setVisible(false);     // مخفی کردن برچسب برنده

    // اتصال خطوط ورودی نام برای فعال کردن بازی
    connect(ui->lineEditName1, &QLineEdit::textChanged, this, &MainWindow::enableGame);
    connect(ui->lineEditName2, &QLineEdit::textChanged, this, &MainWindow::enableGame);

    // اتصال دکمه‌های بازی به هندلر
    for (int i = 1; i <= 25; ++i) {
        QPushButton *button = findChild<QPushButton*>(QString("btn%1").arg(i));
        if (button) {
            connect(button, &QPushButton::clicked, this, &MainWindow::handleButtonClick);
        }
    }

    // اتصال دکمه‌های موافقت و ریست
    connect(ui->btnAgree, &QPushButton::clicked, this, &MainWindow::handleAgree);
    connect(ui->btnReset, &QPushButton::clicked, this, &MainWindow::handleReset);

    // همگام‌سازی Score Board
    connect(ui->lineEditWin1, &QLineEdit::textChanged, this, &MainWindow::syncScoreBoard);
    connect(ui->lineEditLose1, &QLineEdit::textChanged, this, &MainWindow::syncScoreBoard);
    connect(ui->lineEditDraw1, &QLineEdit::textChanged, this, &MainWindow::syncScoreBoard);
    connect(ui->lineEditWin2, &QLineEdit::textChanged, this, &MainWindow::syncScoreBoard);
    connect(ui->lineEditLose2, &QLineEdit::textChanged, this, &MainWindow::syncScoreBoard);
    connect(ui->lineEditDraw2, &QLineEdit::textChanged, this, &MainWindow::syncScoreBoard);

    // مقداردهی اولیه بازی
    resetGame();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::disableAllButtons()
{
    for (int i = 1; i <= 25; ++i) {
        QPushButton *button = findChild<QPushButton*>(QString("btn%1").arg(i));
        if (button) {
            button->setEnabled(false);
        }
    }
}

void MainWindow::enableGame()
{
    bool bothNamesEntered = !ui->lineEditName1->text().isEmpty() && !ui->lineEditName2->text().isEmpty();

    for (int i = 1; i <= 25; ++i) {
        QPushButton *button = findChild<QPushButton*>(QString("btn%1").arg(i));
        if (button) {
            button->setEnabled(bothNamesEntered);
        }
    }

    if (bothNamesEntered) {
        resetGame();
    }
}

void MainWindow::handleButtonClick()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        if(!gameStarted) {
                    ui->lineEditName1->setEnabled(false);
                    ui->lineEditName2->setEnabled(false);
                    gameStarted = true;
                }
        int buttonNumber = button->text().toInt();
        if (currentPlayer == 1) {
            button->setStyleSheet("background-color: white; border: 3px solid red;");
            board[buttonNumber - 1] = 1;
        } else {
            button->setStyleSheet("background-color: white; border: 3px solid blue;");
            board[buttonNumber - 1] = 2;
        }
        button->setEnabled(false);

        // غیرفعال کردن Score Board پس از اولین حرکت
        if (!scoreBoardDisabled) {
            ui->lineEditWin1->setEnabled(false);
            ui->lineEditLose1->setEnabled(false);
            ui->lineEditDraw1->setEnabled(false);
            ui->lineEditWin2->setEnabled(false);
            ui->lineEditLose2->setEnabled(false);
            ui->lineEditDraw2->setEnabled(false);
            scoreBoardDisabled = true;
        }

        checkForWin();
        currentPlayer = (currentPlayer == 1) ? 2 : 1;
    }
}

void MainWindow::checkForWin()
{
    // بررسی برنده در ردیف‌ها
    for (int row = 0; row < 5; ++row) {
        if (board[row*5] != 0 &&
            board[row*5] == board[row*5 + 1] &&
            board[row*5] == board[row*5 + 2] &&
            board[row*5] == board[row*5 + 3] &&
            board[row*5] == board[row*5 + 4]) {
            declareWinner(board[row*5]);
            return;
        }
    }

    // بررسی برنده در ستون‌ها
    for (int col = 0; col < 5; ++col) {
        if (board[col] != 0 &&
            board[col] == board[col + 5] &&
            board[col] == board[col + 10] &&
            board[col] == board[col + 15] &&
            board[col] == board[col + 20]) {
            declareWinner(board[col]);
            return;
        }
    }

    // بررسی قطر اصلی
    if (board[0] != 0 &&
        board[0] == board[6] &&
        board[0] == board[12] &&
        board[0] == board[18] &&
        board[0] == board[24]) {
        declareWinner(board[0]);
        return;
    }

    // بررسی قطر فرعی
    if (board[4] != 0 &&
        board[4] == board[8] &&
        board[4] == board[12] &&
        board[4] == board[16] &&
        board[4] == board[20]) {
        declareWinner(board[4]);
        return;
    }

    // بررسی تساوی
    bool isDraw = true;
    for (int i = 0; i < 25; ++i) {
        if (board[i] == 0) {
            isDraw = false;
            break;
        }
    }
    if (isDraw) {
        declareWinner(0);
    }
}

void MainWindow::declareWinner(int player)
{
    QFont font = ui->lblWinnerTitle->font();
    font.setPointSize(12);
    ui->lblWinnerTitle->setFont(font);

    // ذخیره موقت نتایج
    if (player == 1) {
        tempWin1++;
        tempLose2++;
        ui->lblWinnerTitle->setText("Winner: " + ui->lineEditName1->text());
    } else if (player == 2) {
        tempWin2++;
        tempLose1++;
        ui->lblWinnerTitle->setText("Winner: " + ui->lineEditName2->text());
    } else {
        tempDraw1++;
        tempDraw2++;
        ui->lblWinnerTitle->setText("Draw!");
    }


    ui->lblGameOver->setVisible(true);
    ui->lblWinnerTitle->setVisible(true);
    ui->gridLayoutWidget_4->setVisible(true);
    disableAllButtons();
}

void MainWindow::handleAgree()
{
    if (ui->rbtnYes->isChecked()) {
        // اعمال نتایج موقت به Score Board
        ui->lineEditWin1->setText(QString::number(ui->lineEditWin1->text().toInt() + tempWin1));
        ui->lineEditLose1->setText(QString::number(ui->lineEditLose1->text().toInt() + tempLose1));
        ui->lineEditDraw1->setText(QString::number(ui->lineEditDraw1->text().toInt() + tempDraw1));
        ui->lineEditWin2->setText(QString::number(ui->lineEditWin2->text().toInt() + tempWin2));
        ui->lineEditLose2->setText(QString::number(ui->lineEditLose2->text().toInt() + tempLose2));
        ui->lineEditDraw2->setText(QString::number(ui->lineEditDraw2->text().toInt() + tempDraw2));

        resetGame();
    } else if (ui->rbtnNo->isChecked()) {
        QApplication::quit();
    }

    // ریست کردن مقادیر موقت
    tempWin1 = tempLose1 = tempDraw1 = 0;
    tempWin2 = tempLose2 = tempDraw2 = 0;
}

void MainWindow::handleReset()
{
    // فقط ریست بازی بدون اعمال تغییرات
    tempWin1 = tempLose1 = tempDraw1 = 0;
    tempWin2 = tempLose2 = tempDraw2 = 0;
    resetGame();
}

void MainWindow::resetGame()
{
    // بازنشانی وضعیت بازی
    for (int i = 0; i < 25; ++i) {
        board[i] = 0;
    }
    currentPlayer = 1;

    // فعال کردن مجدد نام بازیکنان
    ui->lineEditName1->setEnabled(true);
    ui->lineEditName2->setEnabled(true);
    gameStarted = false;

    // بازنشانی ظاهر دکمه‌ها
    for (int i = 1; i <= 25; ++i) {
        QPushButton *button = findChild<QPushButton*>(QString("btn%1").arg(i));
        if (button) {
            button->setStyleSheet(""); // حذف استایل‌ها
            button->setEnabled(!ui->lineEditName1->text().isEmpty() && !ui->lineEditName2->text().isEmpty());
        }
    }

    // مخفی کردن عناصر پایان بازی
    ui->lblGameOver->setVisible(false);
    ui->lblWinnerTitle->setVisible(false);
    ui->gridLayoutWidget_4->setVisible(false);

    // فعال‌سازی Score Board برای ویرایش
    ui->lineEditWin1->setEnabled(true);
    ui->lineEditLose1->setEnabled(true);
    ui->lineEditDraw1->setEnabled(true);
    ui->lineEditWin2->setEnabled(true);
    ui->lineEditLose2->setEnabled(true);
    ui->lineEditDraw2->setEnabled(true);
    scoreBoardDisabled = false;

    // تنظیم پیش‌فرض رادیو باتن‌ها
    ui->rbtnYes->setChecked(true);

    // حذف آپدیت مستقیم Score Board از این تابع
    // (دیگر نیازی به آپدیت Score Board در این تابع نیست)
}

void MainWindow::syncScoreBoard()
{
    // جلوگیری از حلقه بی‌نهایت با بلاک کردن سیگنال‌ها
    QSignalBlocker blocker1(ui->lineEditLose2);
    QSignalBlocker blocker2(ui->lineEditLose1);
    QSignalBlocker blocker3(ui->lineEditDraw2);

    ui->lineEditLose2->setText(ui->lineEditWin1->text());
    ui->lineEditLose1->setText(ui->lineEditWin2->text());
    ui->lineEditDraw2->setText(ui->lineEditDraw1->text());
}
