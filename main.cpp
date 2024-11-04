#include <QApplication>
#include <QFileInfo>
#include "download_manager.h"
#include "version_manager.h"
#include "mainwindow.h"
#include <QDebug>

const QString CURRENT_VERSION = "v3.0.0";  // 현재 버전

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 버전 확인 및 업데이트 런처 실행
    checkForUpdate(CURRENT_VERSION);

    // 최신 버전일 경우 가위바위보 게임 실행
    qDebug() << "You are already using the latest version.";

    MainWindow w;
    w.show();
    return app.exec();  // Qt 애플리케이션 실행
}
