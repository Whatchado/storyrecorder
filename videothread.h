#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H
#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif
#include <QObject>
#include <QThread>
#include <QMutex>
#include <QDebug>
#include <QImage>
#include <QTime>

#include "videoencodethread.h"

class VideoThread : public QThread
{
    Q_OBJECT

public:
    int errcode;
    bool running,recording;
    QString cameraname;
    QString audioname;
    QTime t_start;
    VideoThread():
        t_start(QTime::currentTime()),
      m_stop(false)
    {
        running=false;
        recording=false;
            framecounter=0;
            errcode=0;

    }
    void run();
public slots:
    void stop()
    {
        qDebug()<<"Thread::stop called from main thread: "<<currentThreadId();
        QMutexLocker locker(&m_mutex);
        m_stop=true;
    }
    void setFile(const char *filename)
    {
        QMutexLocker locker(&m_mutex);
        int rc=vencode.open(filename);
        vencode.start();
        recording=true;
    }
    void video_encodingFrame(int current,int n)
    {
        emit encodingFrame(current,n);
    };
    void video_encodingDone() {
        emit encodingDone();
    };

private:
    bool m_stop;
    QMutex m_mutex;

    int framecounter;
    VideoEncodeThread vencode;

    double current_time;
    double current_time_offset;
    double recording_time_offset;
signals:
    void frameReady(QImage *img,char *buffer,qint64 t);
    void durationChanged(qint64 t);

    void encodingFrame(int current,int n);
    void encodingDone();

};


#endif // VIDEOTHREAD_H
