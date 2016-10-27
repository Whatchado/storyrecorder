#ifndef VIDEOENCODETHREAD_H
#define VIDEOENCODETHREAD_H

#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QDebug>
#include <QImage>
#include <QTime>

#include "videoencode.h"

class BUFFEREDVIDEOFRAME {
public:
VIDEOIMAGE *img;
double t;
};

class VideoEncodeThread : public QThread
{
    Q_OBJECT

public:
    int errcode;
    bool running;
    VideoEncodeThread();
    void run();
    int open(const char *_filename);
    int writeVideoFrame(VIDEOIMAGE *img, double t);
    int writeAudioFrame(const char *data,int channels,int samples,int bytespersample, double t,int sample_rate)
    {
        if (vencode)
          return vencode->writeAudioFrame(data,channels,samples,bytespersample,t,sample_rate);
    }
    void clearBuffer();
public slots:
    void stop()
    {
        qDebug()<<"Stop encoding"<<currentThreadId();
        QMutexLocker locker(&m_mutex);
        m_stop=true;
    }
private:
    bool m_stop;
    QMutex m_mutex;
    QList<BUFFEREDVIDEOFRAME>imageBuffer;
    int framecounter;
    char *filename;
    VideoEncode *vencode;

signals:
    void encodingFrame(int current,int n);
    void encodingDone();
};


#endif // VIDEOENCODETHREAD_H
