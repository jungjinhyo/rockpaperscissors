#ifndef DOWNLOAD_MANAGER_H
#define DOWNLOAD_MANAGER_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressBar>
#include <QLabel>
#include <QString>

class Downloader : public QWidget {
    Q_OBJECT

public:
    explicit Downloader(QWidget *parent = nullptr);
    ~Downloader();

    void startDownload(const QUrl &url);
    void setFileName(const QString &file);
    void setDownloadDir(const QString &downloadDir);
    QString downloadDir() const;

private slots:
    void updateProgress(qint64 received, qint64 total);
    void finished();
    void calculateTimeRemaining(qint64 received, qint64 total);

private:
    QNetworkAccessManager *m_manager;
    QNetworkReply *m_reply;
    QString m_fileName;
    QString m_downloadDir;
    qint64 m_startTime;
    QProgressBar *m_progressBar;
    QLabel *m_timeLabel;
    QLabel *m_downloadLabel;
    void saveFile(qint64 received, qint64 total);
};

#endif // DOWNLOAD_MANAGER_H
