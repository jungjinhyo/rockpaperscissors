#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>  // 네트워크 요청을 위한 헤더
#include <QNetworkReply>           // 네트워크 응답을 처리하기 위한 헤더

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
    void onUpdateCheckFinished(QNetworkReply *reply);  // 네트워크 응답 처리 슬롯

private:
    Ui::MainWindow *ui;
    QString computerChoice();
    QString determineWinner(const QString &player, const QString &computer);
    void checkForUpdates();  // 최신 버전 확인 함수 선언

    QNetworkAccessManager *networkManager;  // 네트워크 관리 객체
};

#endif // MAINWINDOW_H
