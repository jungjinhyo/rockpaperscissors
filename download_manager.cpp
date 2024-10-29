#include "download_manager.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDateTime>
#include <QDir>
#include <QFile>

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

    // 디렉터리 생성
    QDir dir("installer");
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    m_downloadDir = dir.absolutePath();
}

Downloader::~Downloader() {
    delete m_reply;
    delete m_manager;
}

void Downloader::startDownload(const QUrl &url) {
    // 알림창 표시
    QMessageBox::information(this, tr("Downloading Installer"),
                             tr("최신 버전이 존재합니다. 설치 파일을 다운로드합니다. 잠시만 기다려주세요."));

    // 다운로드 초기화
    m_progressBar->setValue(0);
    m_downloadLabel->setText(tr("Downloading updates..."));
    m_timeLabel->setText(tr("Time remaining: Unknown"));

    QNetworkRequest request(url);
    m_reply = m_manager->get(request);
    m_startTime = QDateTime::currentDateTime().toSecsSinceEpoch();

    connect(m_reply, &QNetworkReply::downloadProgress, this, &Downloader::updateProgress);
    connect(m_reply, &QNetworkReply::finished, this, &Downloader::finished);

    show();
}

void Downloader::setFileName(const QString &file) {
    m_fileName = file.isEmpty() ? "RockPaperScissorsOnlineInstaller_v2.0.0.exe" : file;
}

void Downloader::setDownloadDir(const QString &downloadDir) {
    m_downloadDir = downloadDir;
}

QString Downloader::downloadDir() const {
    return m_downloadDir;
}

void Downloader::updateProgress(qint64 received, qint64 total) {
    if (total > 0) {
        m_progressBar->setMaximum(100);
        m_progressBar->setValue((received * 100) / total);
        calculateTimeRemaining(received, total);
    }
}

void Downloader::calculateTimeRemaining(qint64 received, qint64 total) {
    uint elapsed = QDateTime::currentDateTime().toSecsSinceEpoch() - m_startTime;
    if (elapsed > 0) {
        qreal speed = received / elapsed;
        qreal remainingTime = (total - received) / speed;

        QString timeString;
        if (remainingTime > 3600) {
            timeString = tr("about %1 hours").arg(int(remainingTime / 3600));
        } else if (remainingTime > 60) {
            timeString = tr("about %1 minutes").arg(int(remainingTime / 60));
        } else {
            timeString = tr("%1 seconds").arg(int(remainingTime));
        }

        m_timeLabel->setText(tr("Time remaining: %1").arg(timeString));
    }
}

void Downloader::saveFile(qint64 received, qint64 total) {
    Q_UNUSED(received);
    Q_UNUSED(total);

    QFile file(m_downloadDir + "/" + m_fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        file.write(m_reply->readAll());
        file.close();
    }
}

void Downloader::finished() {
    if (m_reply->error() != QNetworkReply::NoError) {
        QMessageBox::critical(this, tr("Error"), tr("Download failed!"));
        return;
    }

    saveFile(0, 0);
    m_reply->deleteLater();

    QMessageBox::information(this, tr("Download Complete"), tr("Installer downloaded successfully!"));

    // 인스톨러 실행
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_downloadDir + "/" + m_fileName));
    close();
}
