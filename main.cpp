#include <QCoreApplication>
#include "version_manager.h"
#include <QDebug>

const QString CURRENT_VERSION = "v1.0.1";

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    QString latest_version = findLatestVersion();
    qDebug() << "Latest version: " << latest_version;

    int comparison = compareVersions(latest_version, CURRENT_VERSION);
    if (comparison > 0) {
        qDebug() << "A new version is available! Downloading...";
        // 여기에 다운로드 및 설치 함수 호출 가능
    } else {
        qDebug() << "You are already using the latest version.";
    }

    return app.exec();
}
