#ifndef VIDEOENCODE_H
#define VIDEOENCODE_H
#include <QTime>
#include <QMutex>

#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif

/*extern "C" {
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/timestamp.h>
}*/
#include "video_image.h"
extern bool encoding;
extern char *cameraname;
class VideoEncode
{
    AVOutputFormat *fmt;
    AVFormatContext *oc;
    AVStream *audio_st, *video_st;
    AVCodec *audio_codec, *video_codec;
    double audio_time, video_time;
    int ret;

    char *audio_buffer;
    int   audio_buffer_size;
    int   audio_buffer_pos;
    int   audio_buffer_sample_rate;

    int num_videoframes;
public:
    QTime t_start;
    VideoEncode();
    ~VideoEncode();
    int open(const char *filename);
    int writeFrame(VIDEOIMAGE *img, double t);
    int writeAudioFrame(const char *data,int channels,int samples,int bytespersample, double t,int sample_rate);
    double getAudioBufferSize();
    bool isBufferedAudioAvailable();
int getNumBufferedFramesAvailable();
int close();
private:
    double get_audio_frame(int16_t *samples, int frame_size, int nb_channels);
    void write_audio_frame(AVFormatContext *oc, AVStream *st,AVFrame *frame_audio);
    void write_video_frame(AVFormatContext *oc, AVStream *st, VIDEOIMAGE *vimg, uint64_t pts, uint64_t frame_num);
    void writeBufferedAudioFrames();
    FILE *flog;
    QMutex mutexAudio;

};

#endif // VIDEOENCODE_H


