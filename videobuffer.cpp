#include "videobuffer.h"
#include <stdint.h>
//#include <unistd.h>
#include "log.h"

void VideoBuffer::setAudioFormat(int samplerate,int samplesize)
{
    QMutexLocker locker(&m_mutex);

    audiobuffer.setSampleRate(samplerate);
    audiobuffer.setSampleSize(samplesize);
}

    void  VideoBuffer::getAudio(double t,unsigned char *data,size_t ndata)
{
        QMutexLocker locker(&m_mutex);
    audiobuffer.getAudio(t,data,ndata);
}

void VideoBuffer::addAudio(double t, int16_t *data, size_t ndata)
{
    QMutexLocker locker(&m_mutex);
    audiobuffer.addAudio(t,(unsigned char *)data,ndata);
}
void VideoBuffer::addVideo(Frame frame)
{
    QMutexLocker locker(&m_mutex);
    videobuffer.push_back(frame);
    log("VIDEOBUFFER","added Frame %f [n=%i]",frame.t,videobuffer.size());

}
void VideoBuffer::getVideo(Frame &f)
{
    QMutexLocker locker(&m_mutex);
    if(videobuffer.size()<1)
        return;
    f=videobuffer.front();
    videobuffer.pop_front();
}

void VideoBuffer::peekVideo(Frame &f)
{
    QMutexLocker locker(&m_mutex);
    if(videobuffer.size()<1)
        return;
    f=videobuffer.front();
    //videobuffer.pop_front();
}

void VideoBuffer::clear()
{
    log("VIDEOBUFFER","clear");
    QMutexLocker locker(&m_mutex);
    audiobuffer.reset();
    while(videobuffer.size())
    {
        Frame f=videobuffer.front();
        if(f.data) free(f.data);
        videobuffer.pop_front();
    }
    stop=true;
}

bool VideoBuffer::isFull()
{
    QMutexLocker locker(&m_mutex);
    return videobuffer.size()>36;
}
bool VideoBuffer::isEmpty()
{
    QMutexLocker locker(&m_mutex);
    return videobuffer.size()<=0;
}

int VideoBuffer::numFramesAvailable()
{
    QMutexLocker locker(&m_mutex);
    return videobuffer.size();
}
double VideoBuffer::getFirstFrameTime()
{
    QMutexLocker locker(&m_mutex);
    if(videobuffer.size()<1)
        return 0;
    return videobuffer[0].t;
}

