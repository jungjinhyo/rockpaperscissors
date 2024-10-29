#include <QCoreApplication>
#include "version_manager.h"

int main(int argc, char* argv[]) {

    QCoreApplication app(argc, argv);

    // 현재 버전 정보
    const QString CURRENT_VERSION = "v1.0.1";

    // 토큰 불러오기
    QString token = loadTokenFromFile("token.txt");
    if (token.isEmpty()) {
        return -1;  // 토큰이 없으면 종료
    }

    // 버전 정보 업로드
    uploadVersionJson(token, CURRENT_VERSION);

    return app.exec();
}
