#include "version_manager.h"


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
QString getLatestVersionFromDynamoDB(const QString &programName, const QString &getAllLatestVersionsAPI) {
    QList<VersionInfo> versionList;  // 버전 정보를 저장할 리스트

    QNetworkAccessManager manager;

    // AWS API Gateway URL 설정
    QNetworkRequest request{QUrl(getAllLatestVersionsAPI)};

    // GET 요청
    QNetworkReply* reply = manager.get(request);

    // 이벤트 루프를 사용해 요청 완료 대기
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();

        // JSON 파싱
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        if (!jsonDoc.isArray()) {
            qWarning() << "Failed to parse JSON response as array.";
            reply->deleteLater();
            return QString();  // 비어있는 문자열 반환
        }

        QJsonArray jsonArray = jsonDoc.array();
        for (const QJsonValue &value : jsonArray) {
            if (value.isObject()) {
                QJsonObject obj = value.toObject();
                VersionInfo versionInfo;
                versionInfo.programName = obj["programName"].toString();
                versionInfo.latestVersion = obj["latestVersion"].toString();
                versionList.append(versionInfo);
            }
        }

    } else {
        qWarning() << "Failed to fetch updates:" << reply->errorString();
        reply->deleteLater();
        return QString();  // 비어있는 문자열 반환
    }

    reply->deleteLater();

    QString updaterLatestVersion;
    bool updaterFound = false;

    // 요청한 프로그램 이름의 최신 버전 정보 검색
    for (const VersionInfo &versionInfo : versionList) {
        if (versionInfo.programName == programName) {
            updaterLatestVersion = versionInfo.latestVersion;
            updaterFound = true;
            break;
        }
    }

    // 프로그램이 발견되지 않은 경우 경고 로그 출력
    if (!updaterFound) {
        qWarning() << "Program" << programName << "not found in version list.";
        return QString();  // 비어있는 문자열 반환
    }

    return updaterLatestVersion;  // 요청한 프로그램의 최신 버전 반환
}

void uploadVersionToDynamoDB(const QString &programName, const QString &version, const QString &add_or_update_versionAPI) {
    QNetworkRequest request((QUrl(add_or_update_versionAPI)));

    // 헤더 설정
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // 요청 데이터 생성
    QJsonObject json;
    json["programName"] = programName;
    json["version"] = version;
    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    // 네트워크 매니저 설정 및 요청 전송
    QNetworkAccessManager networkManager;
    QNetworkReply *reply = networkManager.put(request, data);

    // 응답을 대기하기 위한 이벤트 루프 설정
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    // 응답 결과 처리
    if (reply->error() == QNetworkReply::NoError) {
        QString response = reply->readAll();
        qDebug() << "uploadVersionToDynamoDB successful";
    } else {
        qDebug() << "Update check failed:" << reply->errorString();
    }

    // 리소스 해제
    reply->deleteLater();
}

// 버전 확인 및 UpdaterLauncher 런처 실행 함수 (Qt Creator 실행 감지 추가)
void checkForUpdate(const QString &programName, const QString &currentVersion, const QString &getAllLatestVersionsAPI, const QString &add_or_update_versionAPI) {
    // 실행 경로에 특정 경로 패턴이 포함된 경우에만 uploadVersionJson 호출
    QFileInfo exeFileInfo(QCoreApplication::applicationFilePath());
    QString exePath = exeFileInfo.absolutePath();

    // Qt Creator에서 실행하는지 특정 경로 패턴 확인
    if (exePath.contains("QtCreatorSource") || exePath.contains("build-")) {
        qDebug() << "Detected Qt Creator environment. Uploading version info.";
        // 버전 업로드 함수 호출
        uploadVersionToDynamoDB(programName, currentVersion, add_or_update_versionAPI);
    }

    QString latestVersion = getLatestVersionFromDynamoDB(programName, getAllLatestVersionsAPI);
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
