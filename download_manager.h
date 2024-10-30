#ifndef DOWNLOAD_MANAGER_H
#define DOWNLOAD_MANAGER_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressBar>
#include <QLabel>
#include <QString>

// 설치 파일을 다운로드하고 실행하는 클래스입니다.
class Downloader : public QWidget {
    Q_OBJECT

public:
    explicit Downloader(QWidget *parent = nullptr);  // 생성자
    ~Downloader();  // 소멸자

    void startDownload(const QUrl &url);  // 다운로드 시작
    void setFileName(const QString &file);  // 파일 이름 설정

private slots:
    void updateProgress(qint64 received, qint64 total);  // 진행 상황 업데이트
    void finished();  // 다운로드 완료 시 호출

private:
    void cleanupReply();  // 응답 정리
    void saveFile();  // 파일 저장
    void handleError(QNetworkReply::NetworkError error);  // 오류 처리
    void calculateTimeRemaining(qint64 received, qint64 total);  // 남은 시간 계산

    QNetworkAccessManager *m_manager;  // 네트워크 매니저
    QNetworkReply *m_reply;  // 네트워크 응답 객체
    QProgressBar *m_progressBar;  // 진행 표시 바
    QLabel *m_timeLabel;  // 남은 시간 표시
    QLabel *m_downloadLabel;  // 다운로드 상태 표시
    QString m_fileName;  // 파일 이름
    QString m_downloadDir;  // 저장 경로
    qint64 m_startTime;  // 시작 시간
};

#endif // DOWNLOAD_MANAGER_H
