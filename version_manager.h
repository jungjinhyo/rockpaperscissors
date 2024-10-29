#ifndef VERSION_MANAGER_H
#define VERSION_MANAGER_H

#include <QString>

// 함수 선언
QString loadTokenFromFile(const QString& filePath);
QString fetchShaForExistingFile(const QString& token, const QString& url);
void uploadVersionJson(const QString& token, const QString current_version);

#endif // VERSION_MANAGER_H
