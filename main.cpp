#include <QApplication>
#include "download_manager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Downloader downloader;
    downloader.setFileName("RockPaperScissorsOnlineInstaller_v2.0.0.exe");
    downloader.startDownload(QUrl("https://jungjinhyo.github.io/rockpaperscissors-installer/v1.1.0/RockPaperScissorsOnlineInstaller.exe"));

    return app.exec();
}
