#include "version_manager.h"
#include <QFileInfo>
#include <QProcess>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QCoreApplication>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QJsonArray>
#include <QTimer>
#include <QApplication>


// 버전 비교 함수
int compareVersions(const QString &v1, const QString &v2) {
    QStringList v1_parts = v1.mid(1).split('.');  // "v" 제거 후 '.'으로 나눔
    QStringList v2_parts = v2.mid(1).split('.');

    int maxLength = qMax(v1_parts.size(), v2_parts.size());

    for (int i = 0; i < maxLength; ++i) {
        int num1 = (i < v1_parts.size()) ? v1_parts[i].toInt() : 0;
        int num2 = (i < v2_parts.size()) ? v2_parts[i].toInt() : 0;

        if (num1 > num2) return 1;  // v1이 더 큼
        if (num1 < num2) return -1; // v2가 더 큼
    }
    return 0;  // 두 버전이 동일
}

// 최신 버전 찾기 함수 (AWS API Gateway 사용)
QString findLatestVersion() {
    QNetworkAccessManager manager;

    // AWS API Gateway에서 최신 버전 정보를 가져오는 요청 설정
    QNetworkRequest request(QUrl("https://9gis6w13h4.execute-api.ap-northeast-2.amazonaws.com/version/get_latest_version"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = manager.get(request);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QString latest_version;

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();

        // JSON 파싱
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        if (!jsonDoc.isObject()) {
            qWarning() << "Failed to parse JSON response.";
            reply->deleteLater();
            return latest_version;
        }

        QJsonObject jsonObj = jsonDoc.object();
        latest_version = jsonObj["latest_version"].toString();  // "latest_version" 필드에서 버전 정보 추출
    } else {
        qWarning() << "Failed to fetch updates:" << reply->errorString();
    }

    reply->deleteLater();
    return latest_version;
}

// 버전 정보 업로드 함수 (AWS API Gateway 사용)
void uploadVersionJson(const QString& current_version) {
    QNetworkAccessManager* manager = new QNetworkAccessManager();

    // AWS API Gateway URL로 요청 설정
    QUrl url("https://9gis6w13h4.execute-api.ap-northeast-2.amazonaws.com/version/add_or_update_version");
    QNetworkRequest request(url);

    // 요청 헤더 설정
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // JSON 데이터 생성 (VersionID 포함)
    QJsonObject newVersion;
    newVersion["VersionID"] = current_version;
    QJsonDocument requestDoc(newVersion);
    QByteArray requestBody = requestDoc.toJson();

    // 이벤트 루프 설정
    QEventLoop loop;

    // PUT 요청 전송
    QNetworkReply* reply = manager->put(request, requestBody);

    // 요청 완료 후 응답 처리
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();  // 네트워크 응답을 기다림

    // 요청 결과 확인
    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "Version uploaded successfully!";
    } else {
        qCritical() << "Failed to upload version:" << reply->errorString();
        qCritical() << "Response:" << reply->readAll();
    }

    reply->deleteLater();
}

// 버전 확인 및 UpdaterLauncher 런처 실행 함수 (Qt Creator 실행 감지 추가)
void checkForUpdate(const QString &currentVersion) {
    // 실행 경로에 특정 경로 패턴이 포함된 경우에만 uploadVersionJson 호출
    QFileInfo exeFileInfo(QCoreApplication::applicationFilePath());
    QString exePath = exeFileInfo.absolutePath();

    // Qt Creator에서 실행하는지 특정 경로 패턴 확인
    if (exePath.contains("QtCreatorSource") || exePath.contains("build-")) {
        qDebug() << "Detected Qt Creator environment. Uploading version info.";
        uploadVersionJson(currentVersion);
    }

    QString latestVersion = findLatestVersion();
    qDebug() << "Latest version: " << latestVersion;
    qDebug() << "Current Version: " << currentVersion;

    int comparison = compareVersions(latestVersion, currentVersion);
    if (comparison > 0) {
        qDebug() << "A new version is available! Launching update...";

        QString launcherPath = QCoreApplication::applicationDirPath() + "/updater/UpdaterLauncher.exe";
        qDebug() << "Launcher path:" << launcherPath;

        // UpdaterLauncher 실행 여부 확인
        if (QFile::exists(launcherPath)) {
            bool result = QProcess::startDetached(launcherPath, QStringList() << latestVersion);
            if (!result) {
                qCritical() << "Failed to start UpdaterLauncher at:" << launcherPath;
            } else {
                qDebug() << "UpdaterLauncher started successfully with version:" << latestVersion;

                // RockPaperScissors.exe를 강제로 종료하는 타이머 설정 (3초 뒤 종료)
                QTimer::singleShot(1000, []() {
                    QApplication::exit(0);
                });
            }
        } else {
            qCritical() << "UpdaterLauncher not found at:" << launcherPath;
        }

        // 프로그램 종료
        QCoreApplication::exit(0);
        return;  // 종료 후 추가 실행 방지
    } else {
        qDebug() << "You are already using the latest version.";
    }
}
