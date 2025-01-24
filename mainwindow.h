#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void enableGame();
    void handleButtonClick();
    void checkForWin();
    void declareWinner(int player);
    void handleAgree();
    void handleReset();
    void resetGame();
    void disableAllButtons();
    void syncScoreBoard();

private:
    Ui::MainWindow *ui;
    int board[25]; // آرایه برای ذخیره وضعیت بازی
    int currentPlayer; // بازیکن فعلی (1 یا 2)
    bool scoreBoardDisabled; // وضعیت غیرفعال بودن Score Board
    bool gameStarted; // افزودن این خط
    int tempWin1;
    int tempLose1;
    int tempDraw1;
    int tempWin2;
    int tempLose2;
    int tempDraw2;
};
#endif // MAINWINDOW_H
