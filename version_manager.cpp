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

// 최신 버전 찾기 함수
QString findLatestVersion() {
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl("https://api.github.com/repos/jungjinhyo/rockpaperscissors-installer/contents/version.json"));
    QNetworkReply* reply = manager.get(request);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QString latest_version;

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        //qDebug() << "Raw API Response:" << response;  // 응답 디버깅

        // JSON 파싱
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        if (!jsonDoc.isObject()) {
            qWarning() << "Failed to parse JSON response.";
            reply->deleteLater();
            return latest_version;
        }

        QJsonObject jsonObj = jsonDoc.object();
        QByteArray content = QByteArray::fromBase64(jsonObj["content"].toString().toUtf8());
        QJsonDocument contentDoc = QJsonDocument::fromJson(content);

        if (!contentDoc.isArray()) {
            qWarning() << "version.json content is not an array.";
            reply->deleteLater();
            return latest_version;
        }

        QJsonArray versions = contentDoc.array();
        QStringList versionList;
        for (const QJsonValue &value : versions) {
            versionList << value.toObject()["version"].toString();
        }

        std::sort(versionList.begin(), versionList.end(), [](const QString &a, const QString &b) {
            return a > b;
        });

        if (!versionList.isEmpty()) {
            latest_version = versionList.first();
        }
    } else {
        qWarning() << "Failed to fetch updates:" << reply->errorString();
    }

    reply->deleteLater();
    return latest_version;
}


// Base64 인코딩 함수
QByteArray base64EncodeJson(const QJsonDocument& jsonDoc) {
    QByteArray jsonData = jsonDoc.toJson();
    return jsonData.toBase64();
}

// GitHub Personal Access Token (PAT) 불러오기, 토큰 로드 함수
QString loadTokenFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Failed to open token file!";
        return QString();
    }
    QTextStream in(&file);
    QString token = in.readLine().trimmed();
    file.close();
    return token;
}


// 기존 파일의 sha 값 가져오기
QString fetchShaForExistingFile(const QString& token, const QString& url) {
    QNetworkAccessManager manager;
    QNetworkRequest request((QUrl(url)));  // 괄호 명시적으로 추가
    request.setRawHeader("Authorization", "token " + token.toUtf8());

    QNetworkReply* reply = manager.get(request);
    while (!reply->isFinished()) {
        qApp->processEvents();  // 이벤트 루프 유지
    }

    QString sha;
    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
        sha = jsonDoc.object()["sha"].toString();
        if (sha.isEmpty()) {
            qCritical() << "SHA value not found!";
        } else {
            qDebug() << "Fetched sha:" << sha;
        }
    } else {
        qCritical() << "Failed to fetch sha:" << reply->errorString();
    }
    reply->deleteLater();
    return sha;
}

// 버전 정보 업로드 함수
void uploadVersionJson(const QString& token, const QString current_version) {
    QNetworkAccessManager* manager = new QNetworkAccessManager();

    // 요청 URL 설정
    QUrl url("https://api.github.com/repos/jungjinhyo/rockpaperscissors-installer/contents/version.json");
    QNetworkRequest request(url);

    // 헤더 설정
    request.setRawHeader("Authorization", "token " + token.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // 기존 버전 정보 가져오기
    QJsonArray versions;
    QString sha = fetchShaForExistingFile(token, url.toString());
    if (!sha.isEmpty()) {
        QNetworkReply* reply = manager->get(request);
        while (!reply->isFinished()) {
            qApp->processEvents();
        }
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
            QByteArray content = QByteArray::fromBase64(jsonDoc.object()["content"].toString().toUtf8());
            QJsonDocument existingDoc = QJsonDocument::fromJson(content);
            versions = existingDoc.array();
        }
        reply->deleteLater();
    }

    // 새로운 버전 정보 추가
    QJsonObject newVersion;
    newVersion["version"] = current_version;
    newVersion["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    versions.append(newVersion);

    // JSON 데이터 생성 및 인코딩
    QJsonDocument updatedDoc(versions);
    QByteArray encodedContent = base64EncodeJson(updatedDoc);

    // 요청 본문 생성
    QJsonObject requestData;
    requestData["message"] = "Add new version";
    requestData["content"] = QString(encodedContent);
    requestData["branch"] = "main";
    if (!sha.isEmpty()) {
        requestData["sha"] = sha;
    }

    QJsonDocument requestDoc(requestData);
    QByteArray requestBody = requestDoc.toJson();

    // PUT 요청 전송
    QNetworkReply* reply = manager->put(request, requestBody);

    QObject::connect(reply, &QNetworkReply::finished, [reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "New version added successfully!";
        } else {
            qCritical() << "Failed to add new version:" << reply->errorString();
            qCritical() << "Response:" << reply->readAll();
        }
        reply->deleteLater();
    });
}

// 버전 확인 및 런처 실행 함수
void checkForUpdate(const QString &currentVersion) {
    QFileInfo exeFileInfo(QCoreApplication::applicationFilePath());
    QString exePath = exeFileInfo.absolutePath();

    if (exePath.contains("build-")) {
        QString token = loadTokenFromFile("token.txt");
        if (token.isEmpty()) {
            qDebug() << "Token file not found or empty.";
            return;
        }

        uploadVersionJson(token, currentVersion);
        qDebug() << "Version uploaded: " << currentVersion;
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
            // 인자로 latestVersion을 전달하며 UpdaterLauncher 실행
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

