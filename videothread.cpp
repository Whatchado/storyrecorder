#include "videothread.h"
//#include "videoin.h"
#include "video_image.h"
#include "audiobuffer.h"

QImage *imgFromFrame(AVFrame *pFrame, int width, int height, int iFrame)
{
    int linesize=pFrame->linesize[0];
    if(width<=0) return NULL;
    if(height<=0) return NULL;
    if(linesize<=0) return NULL;
    if(!pFrame->data[0]) return NULL;
    static uint32_t *imgdata=NULL;
    imgdata=(uint32_t *)realloc(imgdata,width*height*4);
    memset(imgdata,'\0',width*height*4);

    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j++)
        {
            int offset=i*pFrame->linesize[0]+3*j;
            unsigned char *pixel=pFrame->data[0]+offset;
            imgdata[i*width+j]=(pixel[2]<<16) | (pixel[1]<<8) | pixel[0];
        }
    }


    QImage *ret=new QImage((uchar *)imgdata,width,height,width*4,QImage::Format_RGB32);
    //printf("imgp:%p\n",imgdata);
    return ret;
}


void VideoThread::run()
{

    connect(&vencode, SIGNAL(encodingFrame(int,int)), this, SLOT(video_encodingFrame(int,int)));
    connect(&vencode, SIGNAL(encodingDone()), this, SLOT(video_encodingDone()));

    t_start=QTime::currentTime();
    m_stop=false;
    running=true;
    recording=false;
    current_time=0;
    current_time_offset=-1;
    recording_time_offset=-1;

#if defined(__unix__)
    QString filename("video=");
    filename.append(cameraname);
    //filename.append(":audio=/dev/dsp0");
#elif defined(__APPLE__)
    QString filename("default:default");
    //QString filename("1:0");
#else
    QString filename("video=");
    filename.append(cameraname);
    filename.append(":audio=");
    filename.append(audioname);
#endif
    char *filenameSrc=strdup(filename.toLocal8Bit().data());
    printf("xxxx %s\n",filenameSrc);
    //abort();
    AVCodecContext  *pCodecCtx=NULL,*pCodecCtx_audio=NULL;
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    AVCodec *pCodec=NULL,*pCodec_audio=NULL;
#if defined(__unix__)
    AVInputFormat *iformat = av_find_input_format("video4linux2");
#elif defined(__APPLE__)
    AVInputFormat *iformat = av_find_input_format("avfoundation");
#else
    AVInputFormat *iformat = av_find_input_format("dshow");
#endif
    AVFrame *frame_audio = av_frame_alloc();
    printf("--------------------------------OPEN-X----------------------------------------\n");
#if defined(__APPLE__)
    int xx=0;
    for(xx=0;xx<12;xx++) {
        AVDictionary *options=NULL;
        av_dict_set(&options, "framerate", "25", 0);
        //av_dict_set(&options, "video_size", "250x100", 0);
        av_dict_set(&options, "pixel_format","yuyv422",0);

        if(avformat_open_input(&pFormatCtx,filenameSrc,iformat,&options) == 0)
        {
            printf("OSX Video open 25fps\n");
            break;
        }
        av_dict_set(&options, "framerate", "30", 0);
        if(avformat_open_input(&pFormatCtx,filenameSrc,iformat,&options) == 0)
        {
            printf("OSX Video open 30fps\n");
            break;
        }

    av_dict_free(&options);
    QThread::usleep(5000);
}
if(xx==12)
{errcode=-12;return;};
#else
    if(avformat_open_input(&pFormatCtx,filenameSrc,iformat,NULL) != 0)
    {
        errcode=-12;

        return;};

#endif

    if(avformat_find_stream_info(pFormatCtx,NULL) < 0)   {errcode=-13;return;};
    av_dump_format(pFormatCtx, 0, filenameSrc, 0);
    int videoStream = -1;
    int audioStream = -1;
    printf("numstreams:%i----------------------------\n",pFormatCtx->nb_streams);
    for(int i=0; i < pFormatCtx->nb_streams; i++)
    {
        printf("coder_type=%i\n",(int)pFormatCtx->streams[i]->codec->codec_type);
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoStream = i;
        }
        else if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO)
        {
            audioStream = i;
        }
    }
    printf("-------------------------------videostream found %i audio=%i------------------------------\n",videoStream,audioStream);fflush(stdout);
    if(videoStream == -1) {errcode=-14;return;};

    // video codec
    pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    printf("codecid=%i\n",pCodecCtx);
    pCodec =avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL) {printf("codec not found\n");errcode=-15;return;}; //codec not found
    if(avcodec_open2(pCodecCtx,pCodec,NULL) < 0) {errcode=-16;return;};
    // audio codec
    if(audioStream>=0)
    {
        pCodecCtx_audio = pFormatCtx->streams[audioStream]->codec;
        printf("audio codecid=%i\n",pCodecCtx_audio);
        pCodec_audio =avcodec_find_decoder(pCodecCtx_audio->codec_id);
    }
    if(pCodec_audio==NULL) {printf("audio codec not found\n");}
    else
    {
        if(avcodec_open2(pCodecCtx_audio,pCodec_audio,NULL) < 0) {errcode=-16;return;};
    }

    uint8_t *buffer;
    int numBytes;


    VIDEOIMAGE frame(pCodecCtx->width,pCodecCtx->height,pCodecCtx->pix_fmt);
    VIDEOIMAGE frameResize(640,360,AV_PIX_FMT_BGR24);

    int res;
    int frameFinished;
    AVPacket packet;

    while((res = av_read_frame(pFormatCtx,&packet))>=0)
    {
        double pts=0.0;
        if(packet.dts != AV_NOPTS_VALUE) pts = packet.dts*av_q2d(pFormatCtx->streams[packet.stream_index]->time_base);
        if(current_time_offset==-1) current_time_offset=pts;
        current_time=pts-current_time_offset;

        if(packet.stream_index == videoStream)
        {

            avcodec_decode_video2(pCodecCtx,frame.data,&frameFinished,&packet);

            if(frameFinished)
            {
                int h=frame.width*9/16;
                if(h>frame.height) h=frame.height;
                frame.scale(frameResize,h);

                {
                    QMutexLocker locker(&m_mutex);
                    if(recording)
                    {
                        if(recording_time_offset==-1) recording_time_offset=current_time;
                        double record_t=current_time-recording_time_offset;
                        vencode.writeVideoFrame(&frameResize,record_t);
                        emit durationChanged(record_t*1000);

                    }
                }
                char *imgbuffer=NULL;
                QImage *qimg=frameResize.getQImage(&imgbuffer);
                emit frameReady(qimg,imgbuffer,t_start.msecsTo(QTime::currentTime()));
                //delete qimg;free(imgbuffer);
                framecounter++;

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
                //printf("audio t=%f %f\n",current_time,av_q2d(pCodecCtx->time_base));
                size_t unpadded_linesize = frame_audio->nb_samples * av_get_bytes_per_sample((AVSampleFormat )frame_audio->format);
                {
                    QMutexLocker locker(&m_mutex);
                    if(recording)
                    {
                        if(recording_time_offset==-1) recording_time_offset=current_time;
                        double record_t=current_time-recording_time_offset;
                        if(av_get_bytes_per_sample((AVSampleFormat )frame_audio->format)==2)
                        {
                            vencode.writeAudioFrame((char *)frame_audio->extended_data[0],frame_audio->channels,frame_audio->nb_samples,av_get_bytes_per_sample((AVSampleFormat )frame_audio->format),record_t,frame_audio->sample_rate);
                        }
                        else
                        {
                            /* convertFromPlanar((unsigned char *)frame_audio->extended_data[0],frame_audio->channels*frame_audio->nb_samples,
                                    av_get_bytes_per_sample((AVSampleFormat )frame_audio->format));
                                unsigned char *caudiodata=convertSamples((unsigned char *)frame_audio->extended_data[0],
                                frame_audio->channels*frame_audio->nb_samples,
                                av_get_bytes_per_sample((AVSampleFormat )frame_audio->format),2);
                                vencode.writeAudioFrame((char *)caudiodata,frame_audio->channels,frame_audio->nb_samples,2,record_t,frame_audio->sample_rate);
     */
                            int16_t *caudiodata=(int16_t *)malloc(2*2*frame_audio->nb_samples);
                            memset(caudiodata,0,2*2*frame_audio->nb_samples);
                            float *s=(float *)frame_audio->extended_data[0];
                            for(int a=0;a<frame_audio->nb_samples;a++) {
                                int16_t v=32000*s[a*2];
                                caudiodata[a*2]=v;
                                caudiodata[a*2+1]=v;
                            }

                            vencode.writeAudioFrame((char *)caudiodata,2,frame_audio->nb_samples,2,record_t,frame_audio->sample_rate);

                            if(caudiodata) free(caudiodata);

                        }
                    }
                }
                av_free_packet(&packet);
            }
        }

        QMutexLocker locker(&m_mutex);
        if (m_stop) break;
    }

    av_free_packet(&packet);

    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
    if(recording)
    {
        vencode.stop();
        vencode.wait();
    }
    running=false;
    recording=false;

}
