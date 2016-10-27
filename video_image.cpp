#include "video_image.h"
#include <string.h>

VIDEOIMAGE::VIDEOIMAGE()
{
   data=av_frame_alloc();
   width=0;
   height=0;
   format=AV_PIX_FMT_NONE;
   t=0;
   buffer=NULL;
}
VIDEOIMAGE::VIDEOIMAGE(AVFrame *frame)
{
    data=av_frame_alloc();
    *data=*frame;
    width=frame->width;
    height=frame->height;
    format=(AVPixelFormat)frame->format;
    t=frame->pts;
    buffer=NULL;
}
VIDEOIMAGE::VIDEOIMAGE(AVPicture *picture,int _width,int _height,AVPixelFormat _format)
{
    data=av_frame_alloc();
    *((AVPicture *)data)=*picture;
    width=_width;
    height=_height;
    format=_format;
    buffer=NULL;
}

VIDEOIMAGE::VIDEOIMAGE(int _width,int _height,AVPixelFormat _format)
{
    data=av_frame_alloc();
    int size = avpicture_get_size(_format,_width,_height) ;
    buffer = (uint8_t *) av_malloc(size*sizeof(uint8_t));
    avpicture_fill((AVPicture *)data,buffer,_format,_width,_height);

    width=_width;
    height=_height;
    format=_format;

    t=0;
}
VIDEOIMAGE::~VIDEOIMAGE()
{
    if(data) av_free(data);
    data=NULL;
    if(buffer) av_free(buffer);
    buffer=NULL;
}


void VIDEOIMAGE::scale(VIDEOIMAGE &dst)
{
    struct SwsContext * cctx=sws_getCachedContext(NULL,width, height, format,   dst.width,dst.height, dst.format, SWS_BICUBIC, NULL, NULL,NULL);
    sws_scale(cctx,data->data, data->linesize, 0, height, dst.data->data, dst.data->linesize);
    sws_freeContext(cctx);
}
void VIDEOIMAGE::scale(VIDEOIMAGE &dst,int h_copy)
{
    struct SwsContext * cctx=sws_getCachedContext(NULL,width, h_copy, format,   dst.width,dst.height, dst.format, SWS_BICUBIC, NULL, NULL,NULL);
    sws_scale(cctx,data->data, data->linesize, 0, h_copy, dst.data->data, dst.data->linesize);
    sws_freeContext(cctx);
}

QImage *VIDEOIMAGE::getQImage(char **databuffer)
{
    uint32_t *datacopy=getRawImage();

    QImage *ret=new QImage((const uchar *)datacopy,width,height,width*4,QImage::Format_RGB32);
    if(databuffer) *databuffer=(char *)datacopy;

    return ret;
}

uint32_t *VIDEOIMAGE::getRawImage()
{
    int linesize=data->linesize[0];
    if(width<=0) return NULL;
    if(height<=0) return NULL;
    if(linesize<=0) return NULL;
    if(!data->data[0]) return NULL;
    uint32_t *imgdata=(uint32_t *)malloc(width*height*4);
    memset(imgdata,'\0',width*height*4);

    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j++)
        {
            int offset=i*data->linesize[0]+3*j;
            unsigned char *pixel=data->data[0]+offset;
            imgdata[i*width+j]=(pixel[2]<<16) | (pixel[1]<<8) | pixel[0];
        }
    }


    return imgdata;
}
