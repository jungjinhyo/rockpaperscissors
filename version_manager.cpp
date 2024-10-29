#include "version_manager.h"
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>
#include <QByteArray>
#include <QDateTime>

// Base64 인코딩 함수
QByteArray base64EncodeJson(const QJsonDocument& jsonDoc) {
    QByteArray jsonData = jsonDoc.toJson();
    return jsonData.toBase64();
}

// GitHub Personal Access Token (PAT) 불러오기
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
    QNetworkRequest request(QUrl("https://api.github.com/repos/jungjinhyo/rockpaperscissors-installer/contents/version.json"));
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
