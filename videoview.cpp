#include "videoview.h"
#include <QPainter>
#include <QDebug>
VideoView::VideoView(QWidget *parent) :
    QWidget(parent)
{
    img=NULL;
    imgdata=NULL;
    t=0.0;
}


void VideoView::paintEvent(QPaintEvent *evt)
{
    QPainter painter(this);
    if(img)
    {
        int w=this->width();
        int h=this->width()*img->height()/img->width();
        if(h>this->height())
        {
            w=this->height()*img->width()/img->height();
            h=this->height();
        }
        painter.drawImage(QRect((this->width()-w)/2,0,w,h), *img);
    }
   // painter.drawText(QRect(0,0,this->width(),this->height()),QString::number(t));
}
void VideoView::updateImage(QImage *_img,char *_imgdata,double _t)
{
    if(!_img) return;
    t=_t;
    if(imgdata) free(imgdata);
    if(img) delete img;
    img=_img;
    imgdata=_imgdata;
    // update();
    repaint();
    //qDebug()<<"got image "<<_t;
}
