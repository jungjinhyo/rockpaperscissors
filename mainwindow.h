#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void checkForUpdates();
    void onUpdateCheckFinished(QNetworkReply *reply);
    void downloadAndInstallUpdate();
    void onDownloadFinished(QNetworkReply *reply);

    void on_btnRock_clicked();
    void on_btnPaper_clicked();
    void on_btnScissors_clicked();
    void on_btnPlayAgain_clicked();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *networkManager;

    QString computerChoice();
    QString determineWinner(const QString &player, const QString &computer);
    void playGame(const QString &playerChoice);

    // 버전 비교 함수 선언 추가
    bool isVersionNewer(const QString &current, const QString &latest);
};

#endif // MAINWINDOW_H
