#include "config.h"
#include "videoencode.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
extern "C" {
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}
bool encoding=false;

#define STREAM_FRAME_RATE 30
#define STREAM_PIX_FMT    PIX_FMT_YUV420P /* default pix_fmt */
static int sws_flags = SWS_BICUBIC;
/* Add an output stream. */
static AVStream *add_stream(AVFormatContext *oc, AVCodec **codec,
                            enum AVCodecID codec_id)
{
    AVCodecContext *c;
    AVStream *st;
    /* find the encoder */
    *codec = avcodec_find_encoder(codec_id);
    if (!(*codec)) {
        fprintf(stderr, "Could not find encoder for '%s'\n",
                avcodec_get_name(codec_id));
        abort();
    }
    st = avformat_new_stream(oc, *codec);
    if (!st) {
        fprintf(stderr, "Could not allocate stream\n");
        abort();
    }
    st->id = oc->nb_streams-1;
    c = st->codec;
    switch ((*codec)->type) {
    case AVMEDIA_TYPE_AUDIO:
        //c->sample_fmt  = AV_SAMPLE_FMT_FLTP;
        c->sample_fmt  = AV_SAMPLE_FMT_S16;
        //c->sample_fmt  = AV_SAMPLE_FMT_S32P;
        c->bit_rate    = 128000;
        c->sample_rate = 44100;
        c->channels    = 2;
        break;
    case AVMEDIA_TYPE_VIDEO:
        avcodec_get_context_defaults3(c, *codec);
        c->codec_id = codec_id;
        c->bit_rate = 2000000;
        /* Resolution must be a multiple of two. */
        c->width    = 640;
        c->height   = 360;
        /* timebase: This is the fundamental unit of time (in seconds) in terms
         * of which frame timestamps are represented. For fixed-fps content,
         * timebase should be 1/framerate and timestamp increments should be
         * identical to 1. */
        c->time_base.den = STREAM_FRAME_RATE;
        c->time_base.num = 1;
        c->gop_size      = 2; /* emit one intra frame every twelve frames at most */
        c->pix_fmt       = AV_PIX_FMT_YUV420P;
        if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
            /* just for testing, we also add B frames */
           // c->max_b_frames = 2;
        }
        if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
            /* Needed to avoid using macroblocks in which some coeffs overflow.
             * This does not happssen with normal video, it just happens here as
             * the motion of the chroma plane does not match the luma plane. */
            c->mb_decision = 2;
        }

        c->qcompress = 0.6;
        c->qmin = 10;
        c->qmax = 30;
        c->max_qdiff = 4;
        c->i_quant_factor = 0.71;

        break;
    default:
        break;
    }
    /* Some formats want stream headers to be separate. */
    if (oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;
    return st;
}
/**************************************************************/
/* audio output */
static float t, tincr, tincr2;
static uint8_t **src_samples_data;
static int       src_samples_linesize;
static int       src_nb_samples;
static int max_dst_nb_samples;
uint8_t **dst_samples_data;
int       dst_samples_linesize;
int       dst_samples_size;
struct SwrContext *swr_ctx = NULL;
static void open_audio(AVFormatContext *oc, AVCodec *codec, AVStream *st)
{
    AVCodecContext *c;
    int ret;
    c = st->codec;
    /* open it */
    ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
        char buffer[1024];
        av_strerror(ret,buffer,sizeof(buffer));
        fprintf(stderr, "Could not open audio codec:%s\n",buffer );
        abort();
    }
    /* init signal generator */
    t     = 0;
    tincr = 2 * M_PI * 110.0 / c->sample_rate;
    /* increment frequency by 110 Hz per second */
    tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;
    src_nb_samples = c->codec->capabilities & CODEC_CAP_VARIABLE_FRAME_SIZE ?10000 : c->frame_size;
    //src_nb_samples = 22050;
    ret = av_samples_alloc_array_and_samples(&src_samples_data, &src_samples_linesize, c->channels,
                                             src_nb_samples, c->sample_fmt, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate source samples\n");
        abort();
    }
    /* create resampler context */
    if (c->sample_fmt != AV_SAMPLE_FMT_S16) {
        swr_ctx = swr_alloc();
        if (!swr_ctx) {
            fprintf(stderr, "Could not allocate resampler context\n");
            abort();
        }
        /* set options */
        av_opt_set_int       (swr_ctx, "in_channel_count",   c->channels,       0);
        av_opt_set_int       (swr_ctx, "in_sample_rate",     c->sample_rate,    0);
        //av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt",      AV_SAMPLE_FMT_FLTP, 0);
        av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt",      AV_SAMPLE_FMT_S16, 0);
        //av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt",      AV_SAMPLE_FMT_S32P, 0);

        av_opt_set_int       (swr_ctx, "out_channel_count",  c->channels,       0);
        av_opt_set_int       (swr_ctx, "out_sample_rate",    c->sample_rate,    0);
        av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt",     c->sample_fmt,     0);
        /* initialize the resampling context */
        if ((ret = swr_init(swr_ctx)) < 0) {
            fprintf(stderr, "Failed to initialize the resampling context\n");
            abort();
        }
    }
    /* compute the number of converted samples: buffering is avoided
     * ensuring that the output buffer will contain at least all the
     * converted input samples */
    max_dst_nb_samples = src_nb_samples;
    ret = av_samples_alloc_array_and_samples(&dst_samples_data, &dst_samples_linesize, c->channels,
                                             max_dst_nb_samples, c->sample_fmt, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate destination samples\n");
        abort();
    }
    dst_samples_size = av_samples_get_buffer_size(NULL, c->channels, max_dst_nb_samples,
                                                  c->sample_fmt, 0);
}
/* Prepare a 16 bit dummy audio frame of 'frame_size' samples and
 * 'nb_channels' channels. */
double VideoEncode::get_audio_frame(int16_t *samples, int frame_size, int nb_channels)
{
    QMutexLocker locker(&mutexAudio);
    double t_start=double(audio_buffer_pos/4)/double(audio_buffer_sample_rate);
    int j, i;
    int v[2];
    int16_t *q;
    q = samples;

    int16_t *buffer=(int16_t *)&(audio_buffer[audio_buffer_pos]);
    for (j = 0; j < frame_size; j++) {
        v[0] = (int)buffer[j*2];
        v[1] = (int)buffer[j*2+1];
        for (i = 0; i < nb_channels; i++)
            *q++ = v[i];
    }
    audio_buffer_pos+=frame_size*2*2;
    return t_start;
}
void VideoEncode::write_audio_frame(AVFormatContext *oc, AVStream *st,AVFrame *frame_audio)
{
    AVCodecContext *c;
    AVPacket pkt = { 0 }; // data and size must be 0;
    AVFrame *frame = av_frame_alloc();
    int got_packet, ret, dst_nb_samples;
    av_init_packet(&pkt);
    c = st->codec;
    double t=get_audio_frame((int16_t *)src_samples_data[0], src_nb_samples, c->channels);
    /* convert samples from native format to destination codec format, using the resampler */
    if(flog) fprintf(flog,"AUDIO FRAME calc t=%f\n",t);
    /*if (swr_ctx) {
        // compute destination number of samples
        dst_nb_samples = av_rescale_rnd(swr_get_delay(swr_ctx, c->sample_rate) + src_nb_samples,
                                        c->sample_rate, c->sample_rate, AV_ROUND_UP);
        if (dst_nb_samples > max_dst_nb_samples) {
            av_free(dst_samples_data[0]);
            ret = av_samples_alloc(dst_samples_data, &dst_samples_linesize, c->channels,
                                   dst_nb_samples, c->sample_fmt, 0);
            if (ret < 0)
                abort();
            max_dst_nb_samples = dst_nb_samples;
            dst_samples_size = av_samples_get_buffer_size(NULL, c->channels, dst_nb_samples,
                                                          c->sample_fmt, 0);
        }
        // convert to destination format
        ret = swr_convert(swr_ctx,
                          dst_samples_data, dst_nb_samples,
                          (const uint8_t **)frame_audio->extended_data, src_nb_samples);
        if (ret < 0) {
            fprintf(stderr, "Error while converting\n");
            abort();
        }
    } else*/ {
        dst_samples_data[0] = src_samples_data[0];
        dst_nb_samples = src_nb_samples;
    }
    frame->nb_samples = dst_nb_samples;
    //frame->pts=t/av_q2d(st->time_base);
    avcodec_fill_audio_frame(frame, c->channels, c->sample_fmt,
                             dst_samples_data[0], dst_samples_size, 0);
    ret = avcodec_encode_audio2(c, &pkt, frame, &got_packet);
    if (ret < 0) {
        fprintf(stderr, "Error encoding audio frame\n");
        abort();
    }
    if (!got_packet)
        return;
    pkt.stream_index = st->index;
    /* Write the compressed frame to the media file. */

    ret = av_interleaved_write_frame(oc, &pkt);
    if (ret != 0) {
        fprintf(stderr, "Error while writing audio frame\n");
        abort();
    }
    av_frame_free(&frame);
}
static void close_audio(AVFormatContext *oc, AVStream *st)
{
    avcodec_close(st->codec);
    av_free(src_samples_data[0]);
    //av_free(dst_samples_data[0]);
}
/**************************************************************/
/* video output */
static AVFrame *frame;
static AVPicture src_picture, dst_picture;
static int frame_count;
static void open_video(AVFormatContext *oc, AVCodec *codec, AVStream *st)
{
    int ret;
    AVCodecContext *c = st->codec;
    /* open the codec */
    ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
        fprintf(stderr, "Could not open video codec\n");
        abort();
    }
    /* allocate and init a re-usable frame */
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        abort();
    }
    /* Allocate the encoded raw picture. */
    ret = avpicture_alloc(&dst_picture, c->pix_fmt, c->width, c->height);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate picture\n");
        abort();
    }
    /* If the output format is not YUV420P, then a temporary YUV420P
     * picture is needed too. It is then converted to the required
     * output format. */
    if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
        ret = avpicture_alloc(&src_picture, AV_PIX_FMT_YUV420P, c->width, c->height);
        if (ret < 0) {
            fprintf(stderr, "Could not allocate temporary picture\n");
            abort();
        }
    }
    /* copy data and linesize picture pointers to frame */
    *((AVPicture *)frame) = dst_picture;
}

void VideoEncode::write_video_frame(AVFormatContext *oc, AVStream *st,VIDEOIMAGE *vimg,uint64_t pts,uint64_t frame_num)
{
    int ret;
    static struct SwsContext *sws_ctx;
    AVCodecContext *c = st->codec;
    /*if (frame_count >= STREAM_NB_FRAMES) {
        * No more frames to compress. The codec has a latency of a few
         * frames if using B-frames, so we get the last frames by
         * passing the same picture again. *
    } else */
{
        if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
            /* as we only generate a YUV420P picture, we must convert it
             * to the codec pixel format if needed */

            if (!sws_ctx) {
                sws_ctx = sws_getContext(c->width, c->height, AV_PIX_FMT_YUV420P,
                                         c->width, c->height, c->pix_fmt,
                                         sws_flags, NULL, NULL, NULL);
                if (!sws_ctx) {
                    fprintf(stderr,
                            "Could not initialize the conversion context\n");
                    abort();
                }
            }
            //fill_yuv_image(&src_picture, frame_count, c->width, c->height);
            sws_scale(sws_ctx,
                      (const uint8_t * const *)src_picture.data, src_picture.linesize,
                      0, c->height, dst_picture.data, dst_picture.linesize);
        } else {
            if (!sws_ctx) {
            sws_ctx = sws_getContext(vimg->width, vimg->height, vimg->format,
                                     c->width, c->height, c->pix_fmt,
                                     sws_flags, NULL, NULL, NULL);
            }
            sws_scale(sws_ctx,(const uint8_t * const *)vimg->data->data, vimg->data->linesize,
                      0, c->height, dst_picture.data, dst_picture.linesize);

        }
    }
    if (oc->oformat->flags & AVFMT_RAWPICTURE) {
        /* Raw video case - directly store the picture in the packet */
        AVPacket pkt;
        av_init_packet(&pkt);
        pkt.flags        |= AV_PKT_FLAG_KEY;
        pkt.stream_index  = st->index;
        pkt.data          = dst_picture.data[0];
        pkt.size          = sizeof(AVPicture);
        ret = av_interleaved_write_frame(oc, &pkt);
        av_free_packet(&pkt);
    } else {
        AVPacket pkt = { 0 };
        int got_packet;
        av_init_packet(&pkt);
        //pkt.pts=frame_num;
        //pkt.dts=frame_num;
        //pkt.duration=1;
        /* encode the image */
        //frame->pts=c->frame_number;
        ret = avcodec_encode_video2(c, &pkt, frame, &got_packet);

        if (ret < 0) {
            printf("Error encoding video frame\n");
            //    abort();
        }
        /* If size is zero, it means the image was buffered. */
        if (!ret && got_packet && pkt.size) {
            pkt.stream_index = st->index;
            /* Write the compressed frame to the media file. */
            ret = av_interleaved_write_frame(oc, &pkt);
        } else {
            ret = 0;
        }
        if(flog) fprintf(flog,"__ pts3=%li\n",(long)pkt.pts);
        av_free_packet(&pkt);
    }
    if (ret != 0) {
        fprintf(stderr, "Error while writing video frame\n");
        abort();
    }
    frame_count++;
}
static void close_video(AVFormatContext *oc, AVStream *st)
{
    avcodec_close(st->codec);
    av_free(src_picture.data[0]);
    av_free(dst_picture.data[0]);
    av_free(frame);
}
/**************************************************************/
/* media file output */
int VideoEncode::open(const char *filename)
{
    if(flog) fclose(flog);
#ifdef VIDEOENCODINGLOG
    flog=fopen("encoding.log","w");
#endif
    AVOutputFormat *fmt=NULL;
    num_videoframes=0;
    t_start=QTime::currentTime();

    /* allocate the output media context */
    int rc=avformat_alloc_output_context2(&oc, NULL, NULL, filename);
    printf("-%s %p %i\n",filename,oc,rc);
    if (!oc) {
        printf("Could not deduce output format from file extension: using MPEG.\n");
        avformat_alloc_output_context2(&oc, NULL, "mpeg", filename);
    }
    if (!oc) {
        return 1;
    }
    printf("xxxx\n");
    fmt = oc->oformat;
    /* Add the audio and video streams using the default format codecs
     * and initialize the codecs. */
    video_st = NULL;
    audio_st = NULL;
    fmt->audio_codec=AV_CODEC_ID_MP2;
    if (fmt->video_codec != AV_CODEC_ID_NONE) {
        video_st = add_stream(oc, &video_codec, fmt->video_codec);
        //video_st = add_stream(oc, &video_codec, AV_CODEC_ID_VP8);
    }
    if (fmt->audio_codec != AV_CODEC_ID_NONE) {
        //audio_st = add_stream(oc, &audio_codec, fmt->audio_codec);
        audio_st = add_stream(oc, &audio_codec, AV_CODEC_ID_AAC );
    }
    /* Now that all the parameters are set, we can open the audio and
     * video codecs and allocate the necessary encode buffers. */
    if (video_st)
        open_video(oc, video_codec, video_st);
    if (audio_st)
        open_audio(oc, audio_codec, audio_st);
    av_dump_format(oc, 0, filename, 1);
    /* open the output file, if needed */
    if (!(fmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&oc->pb, filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            fprintf(stderr, "Could not open '%s'\n", filename);
            return 1;
        }
    }
    /* Write the stream header, if any. */
    ret = avformat_write_header(oc, NULL);
    if (ret < 0) {
        fprintf(stderr, "Error occurred when opening output file\n");
        return 1;
    }
    if (frame)
        frame->pts = 0;
    return 0;
}
int VideoEncode::writeAudioFrame(const char *data, int channels, int samples, int bytespersample, double t, int sample_rate)
{
    QMutexLocker locker(&mutexAudio);
    int samples_start=0;
    audio_buffer_sample_rate=sample_rate;
    if(t<0)
    {
        samples_start=(-t)*double(sample_rate);
    }

    int samples_copy=samples-samples_start;

    if(samples_copy<=0) return 0;

    int size=samples_copy*bytespersample*channels;
    audio_buffer=(char *)realloc(audio_buffer,audio_buffer_size+size);
    memcpy(&audio_buffer[audio_buffer_size],&(data[samples_start*bytespersample*channels]),size);
    audio_buffer_size+=size;
    return 0;
}
double VideoEncode::getAudioBufferSize()
{
    return double(audio_buffer_size-audio_buffer_pos)/(4.0*audio_buffer_sample_rate);
}


int VideoEncode::getNumBufferedFramesAvailable()
{
    return (audio_buffer_size-audio_buffer_pos)/src_nb_samples*2*2;
}
bool VideoEncode::isBufferedAudioAvailable()
{
    return audio_buffer_size-audio_buffer_pos>src_nb_samples*2*2;
}
void VideoEncode::writeBufferedAudioFrames()
{
    while(isBufferedAudioAvailable())
        write_audio_frame(oc, audio_st,NULL);
}

int VideoEncode::writeFrame(VIDEOIMAGE *img, double t)
{
    if(flog) fprintf(flog,"-------------------------------------------------------------------------------------\n");
    //frame->pts=t/av_q2d(video_st->time_base);//+= 4*av_rescale_q(1, video_st->codec->time_base, video_st->time_base);
    double audio_time = audio_st ? audio_st->pts.val * av_q2d(audio_st->time_base) : 0.0;
    double video_time = video_st ? video_st->pts.val * av_q2d(video_st->time_base) : 0.0;
    if(flog) fprintf(flog,"_VIDEO writeFrame t=%f (%f) at=%f vt=%f\n",t,t*STREAM_FRAME_RATE,audio_time,video_time);
    double calc_video_time=double(num_videoframes)/STREAM_FRAME_RATE;
    do {
        if(flog) fprintf(flog,"-------------------------------------------------------------------------------------\n");
        //frame->pts=calc_video_time/av_q2d(video_st->time_base);
        //video_st->pts=calc_video_time/av_q2d(video_st->time_base);
        //calc_video_time/av_q2d(video_st->time_base);
        int64_t wpts=num_videoframes;
        write_video_frame(oc, video_st,img,wpts,num_videoframes);
        frame->pts += av_rescale_q(1, video_st->codec->time_base, video_st->time_base);
        num_videoframes++;
        calc_video_time = double(num_videoframes-1)/STREAM_FRAME_RATE;
        if(flog) fprintf(flog,"  VIDEO writeFrame num=%i\n",num_videoframes);

        while(isBufferedAudioAvailable()&&(audio_time<calc_video_time))
        {
            if(flog) fprintf(flog,"  AUDIO writeFrame t=%f calc_video_time=%f\n",audio_time,calc_video_time);
            write_audio_frame(oc, audio_st,NULL);
            audio_time = audio_st ? audio_st->pts.val * av_q2d(audio_st->time_base) : 0.0;
        }
    }
    while(calc_video_time<t);

    //writeBufferedAudioFrames();
    return 0;
}
int VideoEncode::close()
{
    /* Write the trailer, if any. The trailer must be written before you
     * close the CodecContexts open when you wrote the header; otherwise
     * av_write_trailer() may try to use memory that was freed on
     * av_codec_close(). */
    av_write_trailer(oc);

    if(oc->oformat)
    {
        if (!(oc->oformat->flags & AVFMT_NOFILE))
            avio_close(oc->pb);
    }
    /* Close each codec. */
    if (video_st)
        close_video(oc, video_st);
    if (audio_st)
        close_audio(oc, audio_st);
    /* free the stream */
    avformat_free_context(oc);
    encoding=false;
    if(flog) fprintf(flog,"CLOSING audiobuffersize=%f\n",getAudioBufferSize());

    if(audio_buffer) {free(audio_buffer);audio_buffer=NULL;};

    return 0;
}


VideoEncode::~VideoEncode()
{
    if(encoding) close();
}

VideoEncode::VideoEncode():
    t_start(QTime::currentTime())
{
    fmt=NULL;
    oc=NULL;
    audio_st=NULL;video_st=NULL;
    audio_codec=NULL;video_codec=NULL;

    audio_buffer=NULL;
    audio_buffer_pos=0;
    audio_buffer_size=0;
    flog=NULL;

    audio_time=0.0; video_time=0.0;
    ret=0;

    audio_buffer_sample_rate=0;

    num_videoframes=0;

}

