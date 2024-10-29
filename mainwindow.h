#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    QString computerChoice();
    QString determineWinner(const QString &player, const QString &computer);
    void playGame(const QString &playerChoice);

    void on_btnRock_clicked();
    void on_btnPaper_clicked();
    void on_btnScissors_clicked();
    void on_btnPlayAgain_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
