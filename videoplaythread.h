#ifndef VIDEOPLAYTHREAD_H
#define VIDEOPLAYTHREAD_H
#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif

#include <QThread>
#include <QTime>
#include <QDebug>
#include <QMutex>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include "videobuffer.h"
#include "videodecodethread.h"

enum VideoPlayerState {VIDEOPLAYER_STOP,VIDEOPLAYER_PLAY,VIDEOPLAYER_PAUSE,VIDEOPLAYER_LOAD};

 Q_DECLARE_METATYPE(VideoPlayerState);


class VideoPlayThread : public QThread
{
    Q_OBJECT
public:
    QTime t_start;
    QString filename;
    bool running;
    explicit VideoPlayThread(QObject *parent = 0);
    void run();
    void waitTillStopped() {while (isRunning()) {usleep(1000);}; };
    bool disable_state_changesignals;
public slots:
    void stop()
    {
        qDebug()<<"Thread::stop called from main thread: "<<currentThreadId();
        while(isRunning())
        {
            {
                QMutexLocker locker(&m_mutex);
                do_stop=true;
            }
            usleep(1000);
        }
        vbuffer.clear();
    }
    void setFile(const char *_filename)
    {
        stop();
        waitTillStopped();
        qDebug()<<"set video "<<_filename;
        setStartPosition(0);setStopPosition(0);
        QMutexLocker locker(&m_mutex);
        tDecode.setVideoBuffer(&vbuffer);
        tDecode.setFile(_filename);

        filename=_filename;
        video_duration=0;
        //m_pause=true;
        do_stop=false;
        running=false;
        //start();
    }
    void setOutput(QWidget *outputwidget);
    void play();
    void pause();
    bool isPaused();
    bool isPlaying();
    bool isStopped();
    double position();
    double duration();
    void seek(double p);
    void setStartPosition(double p);
    void setStopPosition(double p);
    double startPosition() {return pos_start;};
    double stopPosition() {return pos_stop;};
    void durationChange(double d) {video_duration=d;emit durationChanged(d);};
private:
    QMutex m_mutex;

    bool do_stop,do_pause;
    double do_seek;

    double pos_start;
    double pos_stop;

    double video_duration;
    double video_position;

    VideoDecodeThread tDecode;
    VideoBuffer vbuffer;

    QAudioFormat audio_format;
    QAudioDeviceInfo audio_info;
    QAudioOutput *audio;
    QIODevice* audiodevice;

    VideoPlayerState state;

    void setState(VideoPlayerState s) {
        state=s;
        if(disable_state_changesignals) return;
        emit stateChange(state);
    }

signals:
    void frameReady(QImage *img,char *,double t);
    void positionChanged(double t);
    void durationChanged(double d);
    void stateChange(VideoPlayerState state);
};

#endif // VIDEOPLAYTHREAD_H
