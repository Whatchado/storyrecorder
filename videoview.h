#ifndef VIDEOVIEW_H
#define VIDEOVIEW_H

#include <QWidget>
#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif

class VideoView : public QWidget
{
    Q_OBJECT
public:
    explicit VideoView(QWidget *parent = 0);
void paintEvent(QPaintEvent* evt);
private:
QImage *img;
char *  imgdata;
double t;
signals:

public slots:
void updateImage(QImage *, char *_imgdata, double t);
};

#endif // VIDEOVIEW_H
