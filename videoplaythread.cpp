#include "videoplaythread.h"
#include "video_image.h"
#include <QElapsedTimer>
#include "log.h"
#include <QObject>

QObject *parenttest=NULL;

VideoPlayThread::VideoPlayThread(QObject *parent ):
    QThread(parent),
    t_start(QTime::currentTime()),
    do_stop(false),
    do_pause(false)
{
    disable_state_changesignals=false;
    running=false;
    //audio=NULL;
    //audiodevice=NULL;
    pos_start=0.0;pos_stop=0.0;
    video_duration=0.0;
    video_position=0.0;
    do_seek=-1;
    state=VIDEOPLAYER_STOP;
    qRegisterMetaType<VideoPlayerState>("VideoPlayerState");

    //connect(&vbuffer,SIGNAL(frameReady(QImage *,char *,double)),this,SLOT(videobuffer_frameReady(QImage *,char *,double)) );
    connect(&tDecode,SIGNAL(durationChanged(double)),this,SLOT(durationChange(double)));
}

void VideoPlayThread::play()
{
    do_pause=false;
    do_stop=false;
    qDebug()<<"play() do_stop=false";
    if(!running) start();
};
void VideoPlayThread::pause()
{
    do_pause=true;
    if(!running) start();
};
bool VideoPlayThread::isPaused()
{
    if(do_pause) return true;
    return state==VIDEOPLAYER_PAUSE;
};
bool VideoPlayThread::isPlaying()
{
    if(do_pause) return false;
    return state==VIDEOPLAYER_PLAY;
};
bool VideoPlayThread::isStopped()
{
    return state==VIDEOPLAYER_STOP;
};
double VideoPlayThread::position()
{
    return video_position;
};
double VideoPlayThread::duration()
{
    return video_duration;
};
void VideoPlayThread::seek(double p)
{
    log("VIDEOPLAYER","Seek %f do_stop=%i",p,do_stop);
    if(p<pos_start) p=pos_start;
    if(p>pos_stop) p=pos_stop;
    do_seek=p;
    if(!running) start();
};
void VideoPlayThread::setStartPosition(double p)
{
    pos_start=p;
};
void VideoPlayThread::setStopPosition(double p)
{
    pos_stop=p;
};

void VideoPlayThread::setOutput(QWidget *outputwidget)
{
    setState(VIDEOPLAYER_LOAD);
}

void VideoPlayThread::run()
{
    running=true;
do_stop=false;
qDebug()<<"run() do_stop=false";

    log("VIDEOPLAYER","Start Decoding Thread start=%f",pos_start );
    tDecode.setStartPosition(pos_start);
    tDecode.start();
    while(!do_stop)
    {
        setState(VIDEOPLAYER_LOAD);
        log("VIDEOPLAYER","Audio Setup");
        //setup audio
        vbuffer.clear();
        vbuffer.setAudioFormat(44100,2);

        audio_format.setSampleRate(44100);
        audio_format.setChannelCount(1);
        audio_format.setSampleSize(16);
        audio_format.setCodec("audio/pcm");
        audio_format.setByteOrder(QAudioFormat::LittleEndian);
        audio_format.setSampleType(QAudioFormat::SignedInt);
        audio_info = QAudioDeviceInfo::defaultInputDevice();
        if (!audio_info.isFormatSupported(audio_format)) {
            qWarning()<<"default format not supported try to use nearest";
            audio_format = audio_info.nearestFormat(audio_format);
        }
        audio= new QAudioOutput(audio_format);

        while(vbuffer.isEmpty()&&(!do_stop))
            usleep(100);
        setState(VIDEOPLAYER_PLAY);

        audiodevice=audio->start();

        double t=0;
        double pausetime=0.0;
        double play_start=pos_start;
        while(!do_stop)
        {
            log("VIDEOPLAYER","t=%8.3f",t);
            double taudio=double(audio->elapsedUSecs())/1000000-pausetime;
       if(!do_pause) {
            if(audio->bytesFree()>2048)
            {
                uint16_t ab[1024];
                vbuffer.getAudio(t+play_start,(unsigned char *)&ab,1024);

                log("VIDEOPLAYER","%8.3f wrote audio %i t=%f play_start=%f",taudio,audio->bytesFree(),t+play_start,play_start);

                audiodevice->write((char *)&ab,sizeof(ab));
                t+=double(1024)/44110;
            }
            {
                QMutexLocker locker(&m_mutex);
                /*if(vbuffer.numFramesAvailable())
                    log("VIDEOPLAYER","%8.3f videobuffer size=%li t[0]=%f",taudio,vbuffer.numFramesAvailable(),vbuffer.getFirstFrameTime());
                else
                    log("VIDEOPLAYER","%8.3f videobuffer size=%li ",taudio,vbuffer.numFramesAvailable());
*/
                if(vbuffer.numFramesAvailable())
                {
                    //double taudio=startpos+double(audio->elapsedUSecs())/1000000;
                    if(vbuffer.getFirstFrameTime()<=taudio+play_start)
                    {
                        Frame f;
                        vbuffer.getVideo(f);
                        log("VIDEOPLAYER","%8.3f videoemit[f.t=%f]: %f %f %i pausetime=%f buffersize=%i",taudio,f.t,vbuffer.getFirstFrameTime(),taudio+play_start,(vbuffer.getFirstFrameTime()<=taudio,pausetime),vbuffer.numFramesAvailable());
                        if(!f.data) { // NULL data signals video end
                            do_stop=true;
                            qDebug()<<"155) do_stop=true";

                        }
                        else
                        {
                            QImage *qframe=new QImage((const uchar *)f.data,f.w,f.h,f.w*4,QImage::Format_RGB32);
                            video_position=f.t;
                            emit frameReady(qframe,(char *)f.data,f.t);
                            emit positionChanged(f.t);
                        }
                        //emit frameReady(i,f->data,f->t);
                        //f.data=NULL;
                        //delete i;
                    }
                }
            }
}
    //if((!do_pause)&&pos_stop&&(taudio+play_start>pos_stop)) {do_stop=true;}
    if(pos_stop&&(taudio+play_start>pos_stop)) {do_stop=true;    qDebug()<<"173() do_stop=true";}
            usleep(1000);
            //if(do_pause&&(!do_stop))
                if(do_pause)
            {
                setState(VIDEOPLAYER_PAUSE);
                audio->suspend();

                // emit the current frame
                Frame f;
                vbuffer.peekVideo(f);
                if(f.data)
                {
                    unsigned char *ddata=(unsigned char *)malloc(f.h*f.w*4);
                    memcpy(ddata,f.data,f.h*f.w*4);
                    QImage *qframe=new QImage((const uchar *)ddata,f.w,f.h,f.w*4,QImage::Format_RGB32);
                    video_position=f.t;
                    emit frameReady(qframe,(char *)ddata,f.t);
                    emit positionChanged(f.t);
                }

                double pausestart=double(audio->elapsedUSecs())/1000000;
                while(do_pause&&(!do_stop)&&(do_seek==-1))
                {
                    usleep(1000);
                }
                double pauseend=double(audio->elapsedUSecs())/1000000;
                pausetime+=(pauseend-pausestart);
                audio->resume();
                if(!do_stop)
                setState(VIDEOPLAYER_PLAY);
            }
            if(do_seek!=-1)
            {
                do_stop=false;
                double seek_t=do_seek;
                do_seek=-1;

                vbuffer.clear();
                log("VIDEOPLAYER","do seek to %f ------ buffer=%i do_stop=%i",seek_t,vbuffer.numFramesAvailable(),do_stop);
                tDecode.seek(seek_t);

                t=0;
                audio->stop();

                pausetime=0.0;
                play_start=seek_t;
                log("VIDEOPLAYER","do seek to %f buffer=%i",seek_t,vbuffer.numFramesAvailable());
                while(vbuffer.isEmpty()&&(!tDecode.gotVideoAndAudioFrame())&&(!do_stop))
                    usleep(100);
                log("VIDEOPLAYER","do seek buffer=%i",vbuffer.numFramesAvailable());
                Frame f;
                vbuffer.peekVideo(f);
                log("!VIDEOPLAYER","%f",f.t);
                if(f.t==0)
                    qDebug()<<"0";
                video_position=f.t;
                emit positionChanged(f.t);
                audiodevice=audio->start();
            }
        };

    }
    //audio->stop();
    log("VIDEOPLAYER","END");
    tDecode.stop();
    if(audio) delete audio;
    audio=NULL;
    vbuffer.clear();
    setState(VIDEOPLAYER_STOP);
    while (tDecode.isRunning())
      {
        qDebug()<<"tDecode.stop();";
        tDecode.stop();
        tDecode.wait(20);
    }
    running=false;
}
