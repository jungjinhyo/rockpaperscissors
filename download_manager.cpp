#include "download_manager.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QFile>
#include <QProcess>
#include <QDir>
#include <QDateTime>
#include <QDebug>

// 생성자: UI 구성 및 디렉터리 설정
Downloader::Downloader(QWidget *parent)
    : QWidget(parent), m_manager(new QNetworkAccessManager(this)), m_reply(nullptr) {
    setWindowTitle(tr("Installer Download Manager"));

    // UI 설정
    m_progressBar = new QProgressBar(this);
    m_timeLabel = new QLabel(tr("Time remaining: Unknown"), this);
    m_downloadLabel = new QLabel(tr("Downloading updates..."), this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_downloadLabel);
    layout->addWidget(m_progressBar);
    layout->addWidget(m_timeLabel);
    setLayout(layout);

    QMessageBox::information(this, tr("Latest Version is existed"), tr("Your current version is not the latest. Starting download of the latest version."));

    // "install" 디렉터리 생성
    QDir dir("installer");
    if (!dir.exists()) {
        dir.mkpath(".");  // 디렉터리 생성
    }
    m_downloadDir = dir.absolutePath();
}

// 소멸자: 응답 객체 정리 및 네트워크 매니저 삭제
Downloader::~Downloader() {
    cleanupReply();  // 네트워크 요청 정리
    delete m_manager;
}

// 다운로드 시작 함수: URL로부터 파일을 다운로드, 실행합니다.
void Downloader::startDownload(const QUrl &url) {
    // UI 초기화
    m_startTime = QDateTime::currentDateTime().toSecsSinceEpoch();  // 시작 시간 초기화
    m_progressBar->setValue(0);
    m_downloadLabel->setText(tr("Downloading updates..."));
    m_timeLabel->setText(tr("Time remaining: Unknown"));

    // 네트워크 요청 생성
    QNetworkRequest request(url);
    m_reply = m_manager->get(request);

    showNormal();  // 창을 활성화합니다.

    bool connected = connect(m_reply, &QNetworkReply::downloadProgress,
                             this, &Downloader::updateProgress);
    qDebug() << "downloadProgress connected:" << connected;

    // 이벤트 루프를 사용해 다운로드 완료까지 대기
    QEventLoop loop;
    connect(m_reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();  // 이벤트 대기

    // 다운로드 결과 처리
    if (m_reply->error() == QNetworkReply::NoError) {
        saveFile();  // 파일 저장
        QMessageBox::information(this, tr("Download Complete"), tr("Installer downloaded successfully!"));

        // 설치 프로그램 실행
        if (QProcess::startDetached(m_downloadDir + "/" + m_fileName)) {
            qDebug() << "Installer executed successfully.";
        } else {
            qWarning() << "Failed to execute installer.";
        }
    } else {
        handleError(m_reply->error());  // 다운로드 실패 처리
    }

    cleanupReply();  // 응답 객체 정리
}

// 응답 객체 정리 함수
void Downloader::cleanupReply() {
    if (m_reply) {
        disconnect(m_reply, nullptr, this, nullptr);  // 모든 시그널 해제
        m_reply->deleteLater();  // 메모리 해제
        m_reply = nullptr;
    }
}

// 다운로드된 데이터를 파일로 저장합니다.
void Downloader::saveFile() {
    QFile file(m_downloadDir + "/" + m_fileName);  // m_fileName 사용
    if (file.open(QIODevice::WriteOnly)) {
        file.write(m_reply->readAll());  // 데이터 저장
        file.close();
        qDebug() << "Download completed: " << file.fileName();
    } else {
        qWarning() << "Failed to open file for writing.";
    }
}

// 오류 처리 함수
void Downloader::handleError(QNetworkReply::NetworkError error) {
    qWarning() << "Download failed: " << m_reply->errorString();
    QMessageBox::critical(this, tr("Network Error"), tr("Download failed: %1").arg(m_reply->errorString()));
}

// 파일 이름 설정 함수
void Downloader::setFileName(const QString &file) {
    m_fileName = file.isEmpty() ? "RockPaperScissorsOnlineInstaller.exe" : file;
}

// 남은 다운로드 시간 계산 및 UI 업데이트
void Downloader::calculateTimeRemaining(qint64 received, qint64 total) {
    uint elapsed = QDateTime::currentDateTime().toSecsSinceEpoch() - m_startTime;
    if (elapsed > 0) {
        qreal speed = received / elapsed;  // 초당 다운로드 속도
        qreal remainingTime = (total - received) / speed;  // 남은 시간 계산

        QString timeString;
        if (remainingTime > 3600) {
            timeString = tr("about %1 hours").arg(int(remainingTime / 3600));
        } else if (remainingTime > 60) {
            timeString = tr("about %1 minutes").arg(int(remainingTime / 60));
        } else {
            timeString = tr("%1 seconds").arg(int(remainingTime));
        }

        m_timeLabel->setText(tr("Time remaining: %1").arg(timeString));  // UI 업데이트
    }
}

void Downloader::updateProgress(qint64 received, qint64 total) {
    if (total > 0) {
        m_progressBar->setMinimum(0);
        m_progressBar->setMaximum(100);
        m_progressBar->setValue((received * 100) / total);  // 진행률 계산

        // 다운로드 크기 및 남은 시간 계산
        calculateSizes(received, total);
        calculateTimeRemaining(received, total);

        // UI 강제 갱신
        QCoreApplication::processEvents();
    } else {
        m_progressBar->setMinimum(0);
        m_progressBar->setMaximum(0);
        m_progressBar->setValue(-1);
        m_downloadLabel->setText(tr("Downloading Updates..."));
        m_timeLabel->setText(QString("%1: %2").arg(tr("Time Remaining")).arg(tr("Unknown")));
    }
}

void Downloader::calculateSizes(qint64 received, qint64 total) {
    QString totalSize;
    QString receivedSize;

    if (total < 1024)
        totalSize = tr("%1 bytes").arg(total);
    else if (total < 1048576)
        totalSize = tr("%1 KB").arg(total / 1024);
    else
        totalSize = tr("%1 MB").arg(total / 1048576);

    if (received < 1024)
        receivedSize = tr("%1 bytes").arg(received);
    else if (received < 1048576)
        receivedSize = tr("%1 KB").arg(received / 1024);
    else
        receivedSize = tr("%1 MB").arg(received / 1048576);

    m_downloadLabel->setText(tr("Downloading updates") +
                             " (" + receivedSize + " " + tr("of") + " " + totalSize + ")");
}


// 다운로드 완료 시 호출되는 함수
void Downloader::finished() {
    if (!m_reply) return;

    if (m_reply->error() != QNetworkReply::NoError) {
        handleError(m_reply->error());
        return;
    }

    saveFile();

    QMessageBox::information(this, tr("Download Complete"), tr("Installer downloaded successfully!"));
    if (QProcess::startDetached(m_downloadDir + "/RockPaperScissorsOnlineInstaller.exe")) {
        qDebug() << "Installer executed successfully.";
    } else {
        qWarning() << "Failed to execute installer.";
    }

    cleanupReply();
    close();
}
