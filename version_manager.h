#ifndef VERSION_MANAGER_H
#define VERSION_MANAGER_H

#include <QString>
#include <QTimer>
#include <QApplication>

int compareVersions(const QString &v1, const QString &v2);
QString findLatestVersion();
QString loadTokenFromFile(const QString& filePath);
QString fetchShaForExistingFile(const QString& token, const QString& url);
void uploadVersionJson(const QString& token, const QString current_version);
// 버전 확인 및 업데이트 런처 실행 함수
void checkForUpdate(const QString &currentVersion);

#endif // VERSION_MANAGER_H
