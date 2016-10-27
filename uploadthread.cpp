#include "uploadthread.h"

UploadThread::UploadThread(QObject *parent) :
    QThread(parent)
{
}

void UploadThread::setNetwork(QNetworkAccessManager *_nam)
{
}

void UploadThread::run()
{
    //qDebug()<<"test"<<nam;
}
