#ifndef UPLOADTHREAD_H
#define UPLOADTHREAD_H
#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif
#include <QThread>
#include <QNetworkAccessManager>

class UploadThread : public QThread
{
    Q_OBJECT
public:
    explicit UploadThread(QObject *parent = 0);
    void run();
    void setNetwork(QNetworkAccessManager *_nam);
signals:

public slots:
private:
};

#endif // UPLOADTHREAD_H
