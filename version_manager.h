#ifndef VERSION_MANAGER_H
#define VERSION_MANAGER_H

#include <QString>

// 버전 비교 함수
// 두 버전 문자열 v1과 v2를 비교하여 v1이 더 크면 1, v2가 더 크면 -1, 같으면 0을 반환합니다.
int compareVersions(const QString &v1, const QString &v2);

// 최신 버전 찾기 함수 (AWS API Gateway 사용)
// AWS API Gateway에서 최신 버전 정보를 가져와 최신 버전 문자열을 반환합니다.
QString findLatestVersion();

// 버전 정보 업로드 함수 (AWS API Gateway 사용)
// AWS API Gateway에 새로운 버전 정보를 업로드하거나 기존 버전의 timestamp를 갱신합니다.
void uploadVersionJson(const QString& current_version);

// 버전 확인 및 UpdaterLauncher 실행 함수
// 현재 버전을 입력받아 최신 버전과 비교하여, 새 버전이 있으면 UpdaterLauncher를 실행합니다.
void checkForUpdate(const QString &currentVersion);

#endif // VERSION_MANAGER_H
