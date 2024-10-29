#include <QApplication>  // GUI 애플리케이션 실행을 위해 QApplication 사용
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QEventLoop>
#include <QFile>
#include <QProcess>
#include <QDebug>
#include <QRegularExpression>
#include <QStringList>
#include "mainwindow.h"  // 가위바위보 게임 메인 창 헤더 포함

const QString CURRENT_VERSION = "v1.0.6";
const QString UPDATES_URL = "https://jungjinhyo.github.io/rockpaperscissors-installer/";

// 버전 비교 함수
int compareVersions(const QString &v1, const QString &v2) {
    QStringList v1_parts = v1.mid(1).split('.');
    QStringList v2_parts = v2.mid(1).split('.');

    for (int i = 0; i < v1_parts.size() && i < v2_parts.size(); ++i) {
        int num1 = v1_parts[i].toInt();
        int num2 = v2_parts[i].toInt();

        if (num1 > num2) return 1;
        if (num1 < num2) return -1;
    }

    return 0;
}

// 최신 버전 찾기 함수
QString findLatestVersion() {
    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(UPDATES_URL)));

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QString latest_version;
    if (reply->error() == QNetworkReply::NoError) {
        QString html = reply->readAll();
        QRegularExpression regex(R"(v\d+\.\d+\.\d+)");
        QRegularExpressionMatchIterator i = regex.globalMatch(html);

        QStringList versions;
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            versions << match.captured(0);
        }

        std::sort(versions.begin(), versions.end(), [](const QString &a, const QString &b) {
            return a > b;
        });

        if (!versions.isEmpty()) {
            latest_version = versions.first();
        }
    } else {
        qWarning() << "Failed to fetch updates: " << reply->errorString();
    }

    reply->deleteLater();
    return latest_version;
}

// 설치 파일 다운로드 및 실행 함수
void downloadAndRunInstaller(const QString &latest_version) {
    const QString installer_url = UPDATES_URL + latest_version + "/RockPaperScissorsOnlineInstaller.exe";
    const QString download_path = "RockPaperScissorsOnlineInstaller.exe";

    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(installer_url)));

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        QFile file(download_path);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            file.close();
            qDebug() << "Download completed: " << download_path;

            if (QProcess::startDetached(download_path)) {
                qDebug() << "Installer executed successfully.";
            } else {
                qWarning() << "Failed to execute installer.";
            }
        } else {
            qWarning() << "Failed to open file for writing.";
        }
    } else {
        qWarning() << "Download failed: " << reply->errorString();
    }

    reply->deleteLater();
}

// main 함수
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);  // GUI 애플리케이션을 위해 QApplication 사용

    QString latest_version = findLatestVersion();
    qDebug() << "Latest version: " << latest_version;

    int comparison = compareVersions(latest_version, CURRENT_VERSION);
    if (comparison > 0) {
        qDebug() << "A new version is available! Downloading...";
        downloadAndRunInstaller(latest_version);
        return 0;  // 업데이트가 필요하므로 게임을 실행하지 않음
    } else {
        qDebug() << "You are already using the latest version.";
        // 최신 버전일 경우 가위바위보 게임 실행
        MainWindow w;
        w.show();
        return app.exec();
    }
}
