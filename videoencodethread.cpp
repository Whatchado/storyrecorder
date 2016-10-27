#include "videoencodethread.h"

VideoEncodeThread::VideoEncodeThread():
    m_stop(false),
    vencode(),
    imageBuffer()
{
    running=false;
    framecounter=0;
    errcode=0;
    filename=NULL;
    vencode=NULL;
}


int VideoEncodeThread::writeVideoFrame(VIDEOIMAGE *img, double t)
{
    VIDEOIMAGE *vimg=new VIDEOIMAGE(640,360,AV_PIX_FMT_YUV420P);
    img->scale(*vimg);
    {
        QMutexLocker locker(&m_mutex);
        BUFFEREDVIDEOFRAME bvf;
        bvf.img=vimg;bvf.t=t;
        imageBuffer.push_back(bvf);
        framecounter++;
    }
    return 0;
}
int VideoEncodeThread::open(const char *_filename)
{
    if(filename) free(filename);
    filename=strdup(_filename);
    framecounter=0;
    errcode=0;
    clearBuffer();
    return 0;
}
void VideoEncodeThread::run()
{
    m_stop=false;
    vencode= new VideoEncode();
    vencode->open(filename);
    while(1)
    {
        bool things_done=false;
        //qDebug()<<"vencode->isBufferedAudioAvailable()"<<vencode->isBufferedAudioAvailable()<<" size="<<vencode->getAudioBufferSize()<<" num="<<vencode->getNumBufferedFramesAvailable();
        if(vencode->isBufferedAudioAvailable())
        {
            BUFFEREDVIDEOFRAME img;
            img.img=NULL;
            {
                QMutexLocker locker(&m_mutex);
                if(!imageBuffer.isEmpty())
                    img=imageBuffer.takeFirst();
            }
            if(img.img)
            {
                emit encodingFrame(imageBuffer.size(),framecounter);
                vencode->writeFrame(img.img,img.t);
                delete img.img;
                things_done=true;
            }
        }
        // break or wait if nothing to do
        if(!things_done)
        {
            if(m_stop) break;
            usleep(1000);
        }
    }
    vencode->close();
    delete vencode;
    vencode=NULL;
    emit encodingDone();
    clearBuffer();
}


void VideoEncodeThread::clearBuffer()
{
    for(int a=0;a<imageBuffer.size();a++)
    {
        if(imageBuffer[a].img) delete imageBuffer[a].img;
    }
    imageBuffer.clear();
}
