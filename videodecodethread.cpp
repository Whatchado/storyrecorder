#include "videodecodethread.h"
#include <QTime>
#include <QElapsedTimer>
#include "video_image.h"
#include "log.h"

VideoDecodeThread::VideoDecodeThread(QObject *parent) :
    QThread(parent)
{
    is_Running=false;
    doStop=false;
    isPaused=false;
    doPause=false;
    vbuffer=NULL;
    doSeek=-1;
    got_audioframe=false;
    got_videoframe=false;
}
void VideoDecodeThread::setFile(QString const &_filename)
{
    filename=_filename;
    play_start=0.0;
}

void VideoDecodeThread::setVideoBuffer(VideoBuffer *_videobuffer)
{
    vbuffer=_videobuffer;
}

void VideoDecodeThread::seek(double _pos)
{
    //log("VIDEODECODE","**************** seek %f\n",_pos);
    if(_pos==-1) return;
    if(!is_Running)
    {
        start();
    }
    doPause=true;
    while(is_Running&&(!isPaused))
        usleep(100);
    while(is_Running&&(doSeek!=-1)) // wait if other seek in progress
        usleep(100);
    doSeek=_pos;
    vbuffer->clear();
    doPause=false;
    while((doSeek!=-1)&&(is_Running))
        usleep(100);
    //log("VIDEODECODE","*END************ seek %f\n",_pos);
    return;
}
void VideoDecodeThread::stop()
{
    doStop=true;
}
void VideoDecodeThread::pause()
{
    doPause=true;
    while((!isPaused)&&(is_Running))
    {
        usleep(1000);
    }
}

void VideoDecodeThread::waitTillStopped()
{
    while(is_Running)
    {
        usleep(1000);
    }
    return;
}


void VideoDecodeThread::run()
{
    log("VIDEODECODE","play_start=%f",play_start);
    if(play_start)
    {
        doSeek=play_start;
    }
    else
        doSeek=-1;
    doStop=false;
    doPause=false;
    is_Running=true;
    double current_time_offset=-1;
    double seek_pos=0;
    int errcode=0;
    QTime t_start=QTime::currentTime();

    char *filenameSrc=strdup(filename.toLocal8Bit().data());

    AVCodecContext  *pCodecCtx=NULL,*pCodecCtx_audio=NULL;
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    AVCodec *pCodec=NULL,*pCodec_audio=NULL;
    /*#ifdef __unix__
    AVInputFormat *iformat = av_find_input_format("video4linux2");
#else
    AVInputFormat *iformat = av_find_input_format("dshow");
#endif*/
    AVInputFormat *iformat =NULL;
    AVFrame *frame_audio = av_frame_alloc();
    //AVDictionary *options=NULL;
    //av_dict_set(&options, "framerate", "25", 0);

    log("VIDEODECODE","--------------------------------OPEN------------------------------------------");
    if(avformat_open_input(&pFormatCtx,filenameSrc,iformat,NULL) != 0) {errcode=-12;is_Running=false;return;};
    //av_dict_free(&options);

    if(avformat_find_stream_info(pFormatCtx,NULL) < 0)   {errcode=-13;is_Running=false;return;};
    av_dump_format(pFormatCtx, 0, filenameSrc, 0);
    int videoStream = -1;
    int audioStream = -1;
    log("VIDEODECODE","numstreams:%i----------------------------",pFormatCtx->nb_streams);
    for(int i=0; i < pFormatCtx->nb_streams; i++)
    {
        log("VIDEODECODE","coder_type=%i",(int)pFormatCtx->streams[i]->codec->codec_type);
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoStream = i;
        }
        else if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO)
        {
            audioStream = i;
        }
    }
    log("VIDEODECODE","-------------------------------videostream found %i audio=%i ------------------------------",videoStream,audioStream);
    if(videoStream == -1) {errcode=-14;is_Running=false;return;};

    // video codec
    pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    log("VIDEODECODE","codecid=%i",pCodecCtx);
    pCodec =avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL) {printf("codec not found\n");errcode=-15;is_Running=false;return;}; //codec not found
    if(avcodec_open2(pCodecCtx,pCodec,NULL) < 0) {errcode=-16;is_Running=false;return;};
    // audio codec
    if(audioStream>=0)
    {
        pCodecCtx_audio = pFormatCtx->streams[audioStream]->codec;
        log("VIDEODECODE","audio codecid=%i",pCodecCtx_audio);
        pCodec_audio =avcodec_find_decoder(pCodecCtx_audio->codec_id);
    }
    if(pCodec_audio==NULL) {printf("audio codec not found");}
    else
    {
        log("VIDEODECODE","open audio codec");
        if(avcodec_open2(pCodecCtx_audio,pCodec_audio,NULL) < 0) {errcode=-16;is_Running=false;return;};
    }

    uint8_t *buffer;
    int numBytes;

    VIDEOIMAGE frame(pCodecCtx->width,pCodecCtx->height,pCodecCtx->pix_fmt);
    VIDEOIMAGE frameResize(640,360,AV_PIX_FMT_BGR24);

    int res;
    AVPacket packet;

    //av_seek_frame(pFormatCtx,-1,pos_start*AV_TIME_BASE,AVSEEK_FLAG_BACKWARD);

    t_start=QTime::currentTime();
    QElapsedTimer timer;
    timer.start();

    double video_duration=double(pFormatCtx->duration)/AV_TIME_BASE;
    if(video_duration)
    {
        emit durationChanged(video_duration);
        log("VIDEODECODE","duration=%f",video_duration);
    }

    if(doSeek!=-1)
    {
        av_seek_frame(pFormatCtx,-1,doSeek*AV_TIME_BASE,AVSEEK_FLAG_BACKWARD);
        seek_pos=doSeek;
        doSeek=-1;
    }

    log("VIDEODECODE","************************* DECODING seek_pos=%f duration=%f*************************",seek_pos,video_duration);
    while((res = av_read_frame(pFormatCtx,&packet))>=0)
    {
        double pts=0.0;
        if(packet.dts != AV_NOPTS_VALUE) pts = packet.dts*av_q2d(pFormatCtx->streams[packet.stream_index]->time_base);
        if(current_time_offset==-1) current_time_offset=pts;
        double current_time=pts-current_time_offset;

        //        log("VIDEODECODE","frame[%i] %f (pts=%f dts=%f) (offset=%f)",packet.stream_index,current_time,
        //            packet.pts*av_q2d(pFormatCtx->streams[packet.stream_index]->time_base),
        //                packet.dts*av_q2d(pFormatCtx->streams[packet.stream_index]->time_base),current_time_offset);

        //while(pts>double(timer.elapsed())/1000.0)
        //{usleep(1000);};
        if(packet.stream_index == videoStream)
        {
            int frameFinished=1;
            avcodec_decode_video2(pCodecCtx,frame.data,&frameFinished,&packet);
            //log("VIDEODECODE","%i",frameFinished);
            if(frameFinished)
            {
                double pts2 = av_frame_get_best_effort_timestamp ( frame.data )*av_q2d(pFormatCtx->streams[packet.stream_index]->time_base);
                // TODO test for AV_NOPTS_VALUE
                log("VIDEODECODE","- decoded video frame %f (pts=%f dts=%f) pts2=%f",current_time,
                    packet.pts*av_q2d(pFormatCtx->streams[packet.stream_index]->time_base),
                        packet.dts*av_q2d(pFormatCtx->streams[packet.stream_index]->time_base),pts2
                        );
                if(pts2>=seek_pos)
                {
                    int h=frame.width*9/16;
                    if(h>frame.height) h=frame.height;
                    frame.scale(frameResize,h);

                    char *ibuffer=NULL;
                    //QImage *img=frameResize.getQImage(&ibuffer);

                    Frame bframe(pts2,frameResize.getRawImage(),frameResize.width,frameResize.height);

                    vbuffer->addVideo(bframe);
                    if(doSeek!=-1) got_videoframe=true;
                }
                else log("VIDEODECODE","frame video skipped for seek");
                av_free_packet(&packet);
            }
        }
        else if(packet.stream_index == audioStream)
        {
            /* decode audio frame */
            int decoded = packet.size;
            int got_audio_frame = 0;
            int ret = avcodec_decode_audio4(pCodecCtx_audio, frame_audio, &got_audio_frame, &packet);
            if (ret < 0)
                printf("Error decoding audio frame\n");

            decoded = FFMIN(ret, packet.size);
            if (got_audio_frame)
            {
                double packet_t = av_frame_get_best_effort_timestamp ( frame_audio )*av_q2d(pFormatCtx->streams[packet.stream_index]->time_base);
                log("VIDEODECODE","decoded audio frame t=%f",packet_t);
                int samplesize=av_get_bytes_per_sample((AVSampleFormat )frame_audio->format);
                size_t unpadded_linesize = frame_audio->nb_samples * samplesize;
                int16_t *d16=(int16_t *)malloc(frame_audio->nb_samples*sizeof(int16_t));
                if(samplesize==4)
                {
                    for(int a=0;a<frame_audio->nb_samples;a++)
                    {
                        float* decoded_audio_data = reinterpret_cast<float*>(frame_audio->data[0]);
                        d16[a]=(32768)*decoded_audio_data[a];
                    }
                }else if(samplesize==2)
                {
                    for(int a=0;a<frame_audio->nb_samples;a++)
                    {
                        int16_t* decoded_audio_data = reinterpret_cast<int16_t*>(frame_audio->data[0]);
                        d16[a]=decoded_audio_data[a];
                    }
                }
                vbuffer->addAudio(packet_t,d16,frame_audio->nb_samples);
                free(d16);
                if(doSeek!=-1) got_audioframe=true;

            }
        }

        while(vbuffer->isFull()&&(!doStop)&&(!doPause)&&(doSeek==-1))
        {
            usleep(1000);
        }
        while(doPause&&(!doStop)&&(doSeek==-1))
        {
            isPaused=true;
            usleep(100);
        }
        isPaused=false;

        if(doSeek!=-1)
        {
            got_audioframe=false;
            got_videoframe=false;
            log("VIDEODECODE","seek to %f",doSeek);
            av_seek_frame(pFormatCtx,-1,doSeek*AV_TIME_BASE,AVSEEK_FLAG_BACKWARD);
            avcodec_flush_buffers(pCodecCtx);
            avcodec_flush_buffers(pCodecCtx_audio);
            seek_pos=doSeek;
            vbuffer->clear();
            doSeek=-1;
        }
        if (doStop) break;
    }
    log("VIDEODECODE","videoplayer thread end rc=%i",res);

    // set stop frame
    Frame stopFrame;stopFrame.t=0.0;
    vbuffer->addVideo(stopFrame);
    av_free_packet(&packet);

    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    log("VIDEODECODE","videoplayer thread end doStop=%i",doStop);
    doStop=false;
    is_Running=false;

    return;
}
