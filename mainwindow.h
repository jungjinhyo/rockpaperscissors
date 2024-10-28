#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>  // 네트워크 관리 매니저
#include <QNetworkReply>           // 네트워크 응답 처리
#include <QProcess>                // 외부 프로그램 실행

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT  // Qt의 Signal/Slot 기능 사용

public:
    explicit MainWindow(QWidget *parent = nullptr);  // 생성자
    ~MainWindow();  // 소멸자

private slots:
    void checkForUpdates();  // 최신 버전 확인 함수
    void onUpdateCheckFinished(QNetworkReply *reply);  // 업데이트 확인 후 처리 슬롯
    void downloadAndInstallUpdate();  // 업데이트 다운로드 및 설치 함수
    void onDownloadFinished(QNetworkReply *reply);  // 다운로드 완료 후 처리 슬롯

    // 게임 로직과 연결된 슬롯
    void on_btnRock_clicked();
    void on_btnPaper_clicked();
    void on_btnScissors_clicked();
    void on_btnPlayAgain_clicked();

private:
    Ui::MainWindow *ui;  // UI 객체
    QNetworkAccessManager *networkManager;  // 네트워크 관리 매니저 객체

    // 게임 로직 함수
    QString computerChoice();  // 컴퓨터의 선택 결정 함수
    QString determineWinner(const QString &player, const QString &computer);  // 승리자 결정 함수
    void playGame(const QString &playerChoice);  // 게임 실행 함수
};

#endif // MAINWINDOW_H
