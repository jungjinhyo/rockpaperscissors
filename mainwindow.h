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
    void playGame(const QString &playerChoice);
    void on_btnRock_clicked();
    void on_btnPaper_clicked();
    void on_btnScissors_clicked();
    void on_btnPlayAgain_clicked();

private:
    Ui::MainWindow *ui;
    QString computerChoice();
    QString determineWinner(const QString &player, const QString &computer);
};

#endif // MAINWINDOW_H
