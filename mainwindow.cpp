#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <random>
#include <chrono>
#include <QMessageBox>
#include <QXmlStreamReader>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>
#include <QStandardPaths>  // QStandardPaths 사용을 위한 헤더 파일
#include <QFile>  // QFile 사용을 위한 헤더 파일

const QString CURRENT_VERSION = "1.04";  // 현재 버전
const QString UPDATES_URL = "https://jungjinhyo.github.io/rockpaperscissors-installer/latest/repository/Updates.xml";  // 업데이트 XML 경로
const QString INSTALLER_URL = "https://jungjinhyo.github.io/rockpaperscissors-installer/latest/RockPaperScissorsOnlineInstaller.exe";  // 설치 파일 URL

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    networkManager = new QNetworkAccessManager(this);  // 네트워크 관리 객체 생성

    // 네트워크 요청 완료 시 onUpdateCheckFinished 호출
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onUpdateCheckFinished);

    // 프로그램 시작 시 최신 버전 확인
    checkForUpdates();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::downloadAndInstallUpdate() {
    QUrl url(INSTALLER_URL);
    QNetworkRequest request(url);

    // 네트워크 요청 완료 시 onDownloadFinished 호출
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onDownloadFinished);

    networkManager->get(request);  // 설치 파일 다운로드 시작
}

void MainWindow::onDownloadFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        // 임시 파일 경로에 다운로드된 파일 저장
        QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/installer.exe";
        QFile file(tempPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            file.close();

            // 설치 파일 실행
            QProcess::startDetached(tempPath);  // installer.exe 실행
            QApplication::quit();  // 기존 프로그램 종료
        } else {
            QMessageBox::warning(this, "Error", "Failed to save the installer.");
        }
    } else {
        QMessageBox::warning(this, "Error", "Failed to download the update.");
    }
    reply->deleteLater();  // 응답 객체 해제
}

void MainWindow::checkForUpdates() {
    QUrl url(UPDATES_URL);
    QNetworkRequest request(url);
    networkManager->get(request);  // 업데이트 XML 파일 요청
}

void MainWindow::onUpdateCheckFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QXmlStreamReader xml(response);

        QString latestVersion;

        // XML 파싱을 통해 최신 버전 정보 추출
        while (!xml.atEnd() && !xml.hasError()) {
            xml.readNext();
            if (xml.name().toString() == QStringLiteral("ApplicationVersion") && xml.isStartElement()) {
                latestVersion = xml.readElementText();
                break;
            }
        }

        // 최신 버전과 현재 버전 비교
        if (latestVersion > CURRENT_VERSION) {
            QMessageBox::StandardButton reply =
                QMessageBox::question(this, "Update Available",
                                      "A new version (" + latestVersion + ") is available. "
                                                                          "Would you like to update now?",
                                      QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                downloadAndInstallUpdate();  // 업데이트 다운로드 및 설치 시작
            } else {
                QMessageBox::information(this, "Up to Date", "You are using the latest version.");
            }
        } else {
            QMessageBox::information(this, "Up to Date", "You are using the latest version.");
        }
    } else {
        QMessageBox::warning(this, "Error", "Failed to check for updates.");
    }
    reply->deleteLater();  // 응답 객체 해제
}

QString MainWindow::computerChoice() {
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribution(0, 2);

    int choice = distribution(generator);
    if (choice == 0) return "Rock";
    else if (choice == 1) return "Paper";
    else return "Scissors";
}

QString MainWindow::determineWinner(const QString &player, const QString &computer) {
    if (player == computer) return "It's a tie!";
    if ((player == "Rock" && computer == "Scissors") ||
        (player == "Paper" && computer == "Rock") ||
        (player == "Scissors" && computer == "Paper"))
        return "You win!";
    else
        return "You lose!";
}

void MainWindow::playGame(const QString &playerChoice) {
    QString compChoice = computerChoice();
    QString result = determineWinner(playerChoice, compChoice);

    QString message = QStringLiteral("Your chose: %1\nComputer chose: %2\n%3")
                          .arg(playerChoice, compChoice, result);

    ui->lblResult->setText(message);
}

void MainWindow::on_btnRock_clicked() {
    playGame("Rock");
}

void MainWindow::on_btnPaper_clicked() {
    playGame("Paper");
}

void MainWindow::on_btnScissors_clicked() {
    playGame("Scissors");
}

void MainWindow::on_btnPlayAgain_clicked() {
    ui->lblResult->setText("Choose your move:");
}
