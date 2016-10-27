#ifndef VIDEOPREVIEW_H
#define VIDEOPREVIEW_H
#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif

#include <QImage>
#include <QThread>

#define VIDEOPREVIEW


extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/mathematics.h"
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
}

void clearVideoFrameCache();
QImage *getVideoFrame(const char *file,double t);
QImage *getVideoFrameCached(const char *file,double t);

class VideoPreviewThread : public QThread
{
    Q_OBJECT
public:
    bool running;
    VideoPreviewThread(QObject *parent = 0);
    void run();
    void addPreview(QString const &file,double t);
private:
    QVector<QString > do_filename;
    QVector<double  > do_t;
signals:
    void videoPreviewUpdate();
};

extern VideoPreviewThread tVideoPreview;
#endif // VIDEOPREVIEW_H
