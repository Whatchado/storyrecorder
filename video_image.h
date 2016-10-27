#ifndef VIDEOFRAME_H
#define VIDEOFRAME_H

extern "C" {

#define snprintf _snprintf

#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/timestamp.h>
#include <libavutil/avconfig.h>
}
#include <QImage>
class VIDEOIMAGE
{
public:
    VIDEOIMAGE();
    VIDEOIMAGE(AVFrame *frame);
    VIDEOIMAGE(AVPicture *picture,int width,int height,AVPixelFormat format);
    VIDEOIMAGE(int width,int height,AVPixelFormat format);
    ~VIDEOIMAGE();

    QImage *getQImage(char **databuffer);
    uint32_t *getRawImage();
    void scale(VIDEOIMAGE &dst);
    void scale(VIDEOIMAGE &dst,int h_copy);
    int width;
    int height;
    AVPixelFormat format;
    double t;
    AVFrame *data;
private:
    uint8_t *buffer;
};

#endif // VIDEOFRAME_H
