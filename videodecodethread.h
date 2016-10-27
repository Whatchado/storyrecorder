#ifndef VIDEODECODETHREAD_H
#define VIDEODECODETHREAD_H

#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif

#include <QThread>
#include <QString>
#include <QMutex>
#include "videobuffer.h"

class VideoDecodeThread : public QThread
{
    Q_OBJECT
public:
    explicit VideoDecodeThread(QObject *parent = 0);
    void run();
    void setStartPosition(double _play_start) {play_start=_play_start;};

    void setFile(QString const &_filename);
    void setVideoBuffer(VideoBuffer *videobuffer);
    void pause();
    void seek(double _pos);
    void stop();
    void waitTillStopped();
    bool gotVideoAndAudioFrame() {return got_videoframe&&got_audioframe;};
signals:

public slots:

private:
    bool is_Running;
    bool doStop;
    bool isPaused;
    bool doPause;
    double doSeek;
    double play_start;
    QString filename;
    VideoBuffer *vbuffer;

    bool got_videoframe;
    bool got_audioframe;

    QMutex m_mutex;
signals:
    void durationChanged(double d);

};

#endif // VIDEODECODETHREAD_H
