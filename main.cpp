#include <QApplication>
#include "download_manager.h"  // 다운로드 관련 클래스 정의된 헤더
#include "version_manager.h"   // 버전 관련 클래스 정의된 헤더
#include "mainwindow.h"        // 가위바위보 게임
#include <QDebug>

const QString CURRENT_VERSION = "v3.3.2"; // 현재 버전. 코드를 수정할 때마다 최신 버전으로 업데이트해야 함.

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);  // Qt 애플리케이션 초기화

    // 최신 버전을 확인하는 함수 호출
    QString latest_version = findLatestVersion();
    qDebug() << "Latest version: " << latest_version;  // 최신 버전 출력

    // 최신 버전과 현재 버전을 비교
    int comparison = compareVersions(latest_version, CURRENT_VERSION);

    if (comparison > 0) {  // 최신 버전이 현재 버전보다 높을 경우
        qDebug() << "A new version is available! Downloading...";  // 새 버전이 존재한다는 메시지 출력

        Downloader downloader;  // 다운로드 매니저 객체 생성

        // 다운로드할 파일명을 최신 버전에 맞게 설정
        downloader.setFileName(QString("RockPaperScissorsOnlineInstaller_%1.exe").arg(latest_version));
        // 최신 버전에 맞는 URL 생성하여 다운로드 시작
        downloader.startDownload(QUrl(QString("https://jungjinhyo.github.io/rockpaperscissors-installer/%1/RockPaperScissorsOnlineInstaller.exe").arg(latest_version)));
    } else {
        // 현재 사용 중인 버전이 최신 버전일 경우 메시지 출력
        qDebug() << "You are already using the latest version.";

        MainWindow w;   //가위바위보 게임 실행, 화면 보여줌
        w.show();
        return app.exec();  // Qt 애플리케이션 실행
    }    
}
