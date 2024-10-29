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
#include <QEventLoop>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
#include <algorithm>
#include <QDebug>

// 버전 비교 함수
int compareVersions(const QString &v1, const QString &v2) {
    QStringList v1_parts = v1.mid(1).split('.');
    QStringList v2_parts = v2.mid(1).split('.');

    for (int i = 0; i < v1_parts.size() && i < v2_parts.size(); ++i) {
        int num1 = v1_parts[i].toInt();
        int num2 = v2_parts[i].toInt();

        if (num1 > num2) return 1;
        if (num1 < num2) return -1;
    }
    return 0;
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

// 버전 정보 업로드 함수
void uploadVersionJson(const QString& token, const QString current_version) {
    QNetworkAccessManager* manager = new QNetworkAccessManager();
    QUrl url("https://api.github.com/repos/jungjinhyo/rockpaperscissors-installer/contents/version.json");
    QNetworkRequest request(url);

    request.setRawHeader("Authorization", "token " + token.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonArray versions;
    QString sha = findLatestVersion();
    if (!sha.isEmpty()) {
        versions.append(current_version);
    }

    QJsonDocument updatedDoc(versions);
    QByteArray encodedContent = updatedDoc.toJson().toBase64();

    QJsonObject requestData;
    requestData["message"] = "Add new version";
    requestData["content"] = QString(encodedContent);
    requestData["branch"] = "main";

    QJsonDocument requestDoc(requestData);
    QByteArray requestBody = requestDoc.toJson();

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
