#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <random>
#include <chrono>
#include <QMessageBox>
#include <QXmlStreamReader>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>
#include <QStandardPaths>
#include <QFile>
#include <QDebug>  // 디버깅 메시지 출력을 위한 헤더 파일

const QString CURRENT_VERSION = "1.04";  // 현재 버전
const QString UPDATES_URL = "https://jungjinhyo.github.io/rockpaperscissors-installer/latest/repository/Updates.xml";  // 업데이트 XML 경로
const QString INSTALLER_URL = "https://jungjinhyo.github.io/rockpaperscissors-installer/latest/RockPaperScissorsOnlineInstaller.exe";  // 설치 파일 URL

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    networkManager = new QNetworkAccessManager(this);

    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onUpdateCheckFinished);

    checkForUpdates();  // 프로그램 시작 시 최신 버전 확인
}

MainWindow::~MainWindow() {
    delete ui;
}

// 버전 비교 함수: 최신 버전인지 확인
bool MainWindow::isVersionNewer(const QString &current, const QString &latest) {
    QStringList currentParts = current.split('.');
    QStringList latestParts = latest.split('.');

    for (int i = 0; i < std::max(currentParts.size(), latestParts.size()); ++i) {
        int currentPart = (i < currentParts.size()) ? currentParts[i].toInt() : 0;
        int latestPart = (i < latestParts.size()) ? latestParts[i].toInt() : 0;

        if (latestPart > currentPart) return true;
        if (currentPart > latestPart) return false;
    }
    return false;  // 동일한 버전인 경우
}

void MainWindow::downloadAndInstallUpdate() {
    QUrl url(INSTALLER_URL);
    QNetworkRequest request(url);

    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onDownloadFinished);

    networkManager->get(request);  // 설치 파일 다운로드 시작
}

void MainWindow::onDownloadFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/RockPaperScissorsOnlineInstaller.exe";
        QFile file(tempPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            file.close();

            // 디버깅 메시지로 경로 출력
            qDebug() << "Installer saved at:" << tempPath;

            bool started = QProcess::startDetached(tempPath);  // 설치 파일 실행
            if (started) {
                qDebug() << "Installer started successfully.";
            } else {
                qDebug() << "Failed to start installer.";
            }

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

        // XML 파싱 과정 디버깅 메시지
        qDebug() << "XML Response:" << response;

        while (!xml.atEnd() && !xml.hasError()) {
            xml.readNext();
            if (xml.name().toString() == QStringLiteral("ApplicationVersion") && xml.isStartElement()) {
                latestVersion = xml.readElementText();
                break;
            }
        }

        // 최신 버전 정보 디버깅 메시지
        qDebug() << "Current Version:" << CURRENT_VERSION;
        qDebug() << "Latest Version:" << latestVersion;

        if (isVersionNewer(CURRENT_VERSION, latestVersion)) {
            QMessageBox::StandardButton result =
                QMessageBox::question(this, "Update Available",
                                      "A new version (" + latestVersion + ") is available. "
                                                                          "Would you like to update now?",
                                      QMessageBox::Yes | QMessageBox::No);
            if (result == QMessageBox::Yes) {
                downloadAndInstallUpdate();  // 업데이트 다운로드 및 설치 시작
                return;  // 이후 코드 실행 방지
            }
        }

        QMessageBox::information(this, "Up to Date", "You are using the latest version.");
    } else {
        QMessageBox::warning(this, "Error", "Failed to check for updates.");
    }
    reply->deleteLater();  // 응답 객체 해제
}

// 게임 로직 구현
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

    QString message = QStringLiteral("You chose: %1\nComputer chose: %2\n%3")
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
