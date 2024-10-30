#ifndef VERSION_MANAGER_H
#define VERSION_MANAGER_H

#include <QString>

int compareVersions(const QString &v1, const QString &v2);
QString findLatestVersion();
QString loadTokenFromFile(const QString& filePath);
QString fetchShaForExistingFile(const QString& token, const QString& url);
void uploadVersionJson(const QString& token, const QString current_version);

#endif // VERSION_MANAGER_H
