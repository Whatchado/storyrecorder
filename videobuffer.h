#ifndef VIDEOBUFFER_H
#define VIDEOBUFFER_H
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
#include "audiobuffer.h"
#include <stdint.h>

class Frame {
public:
    uint32_t *data;
    int w;
    int h;
    double t;
    Frame() {data=NULL;w=0;h=0;t=0.0;};
    Frame(double _t,uint32_t *_data,int _w,int _h) {t=_t;data=_data;w=_w;h=_h;};
    ~Frame() {};
};

class VideoBuffer : public QThread
{
    Q_OBJECT
public:
    bool running;
    explicit VideoBuffer(QObject *parent = 0):
        QThread(parent)
    {
    }
    ~VideoBuffer() {clear();}
    void addAudio(double t,int16_t *data,size_t ndata);
    void addVideo(Frame frame);
    void getVideo(Frame &f);
    void peekVideo(Frame &f);
    void setAudioFormat(int samplerate,int samplesize);
    void getAudio(double t,unsigned char *data,size_t ndata);
    void clear();
    bool isFull();
    bool isEmpty();
    int  numFramesAvailable();
    double getFirstFrameTime();
    void setPause(bool _p) {pause=_p;};
public slots:
private:
    bool stop,pause;
    bool audio_enabled;
    QMutex m_mutex;

    AUDIOBUFFER audiobuffer;
    QVector<Frame> videobuffer;

};

#endif // VIDEOBUFFER_H
