#include <QApplication>
#include <QFileInfo>
#include "version_manager.h"
#include "mainwindow.h"
#include <QDebug>

const QString CURRENT_VERSION = "v5.1.0";  // 현재 버전
const QString PROGRAM_NAME = "rockpaperscissors"; // 프로그램 이름
const QString getAllLatestVersionsAPI = "https://0b3xq5xnkd.execute-api.ap-northeast-2.amazonaws.com/version/getAllLatestVersions";
const QString add_or_update_versionAPI = "https://0b3xq5xnkd.execute-api.ap-northeast-2.amazonaws.com/version/AddOrUpdateVersion";

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 버전 확인 및 업데이트 런처 실행
    checkForUpdate(PROGRAM_NAME, CURRENT_VERSION, getAllLatestVersionsAPI, add_or_update_versionAPI);

    // // 최신 버전일 경우 가위바위보 게임 실행
    qDebug() << "You are already using the latest version.";

    MainWindow w;
    w.show();

    return app.exec();  // Qt 애플리케이션 실행
}
