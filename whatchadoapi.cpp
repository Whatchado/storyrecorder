#include "whatchadoapi.h"
#include <QObject>
#include <QUrl>
#include <QNetworkReply>
#include <QDebug>
#include <QMap>
#include <QTime>
#include <QFile>
#include <QCryptographicHash>
#include <QHttpMultiPart>
#include <time.h>
int api_state=API_STATE_NONE;
QObject *progressTarget=NULL;
WhatchadoApi api;


class WhatchadoApiParam {
public:
    QMap<QString,QString> param;
    QString  nonce;
    QByteArray K,I,O;
    int randInt(int low, int high) {
        static int randinit=0;
        if(!randinit) {qsrand(time(NULL)*1000+QTime::currentTime().msec());randinit=1;};
        return qrand() % ((high + 1) - low) + low;
    }
    WhatchadoApiParam() {
        K = QByteArray::fromHex(REPLACE_THIS_WITH_A_VALID_API_KEY);
        for(int a=0;a<16;a++) nonce.append((char)randInt('0','9'));
        for(int a=0;a<64;a++) I.append("\x36");
        for(int a=0;a<64;a++) O.append("\x5c");
        set("version","1.1");
    }
    ~WhatchadoApiParam() {
    }
    QByteArray Xor(const QByteArray& A, const QByteArray& B)
    {
        if(B.isEmpty()) return A;

        QByteArray r;
        for(int i = 0 , j = 0; i < A.length(); ++i , ++j)
        {
            if(j == B.length()) j = 0;
            r.append(A.at(i) ^ B.at(j));
        }
        return r;
    }

    void set(const QString &key,const QString &value)
    {
        param[key]=value;
    }
    QString result()
    {
        param["nonce"]=nonce;
        QString res;
        QByteArray M;
        //M+="mail=";
        //M+=QUrl::toPercentEncoding(mail);
        QMapIterator<QString, QString> i(param);
        while (i.hasNext()) {
            i.next();
            if(!M.isEmpty()) M+='&';
            M+=i.key();
            M+="=";
            M+=i.value();
        }
        qDebug()<<M;

        QByteArray KI=Xor(K,I);
        QByteArray KO=Xor(K,O);
        KI.append(M);
        QCryptographicHash hash(QCryptographicHash::Sha256);
        hash.addData(KI);
        KO.append(hash.result());
        hash.reset();
        hash.addData(KO);
        QMapIterator<QString, QString> p(param);
        while (p.hasNext()) {
            p.next();
            if(!res.isEmpty()) res+='&';
            res+=p.key();
            res+="=";
            res+=QUrl::toPercentEncoding(p.value());
        }
        res.append("&hmac=");
        res.append(hash.result().toPercentEncoding());
        return res;
    }
};


int WhatchadoApi::registerAccount(QNetworkAccessManager *nam,QString first_name,QString last_name,QString mail,QString password,QString lang)
{
    WhatchadoApiParam param;
    param.set("first_name",first_name);
    param.set("last_name",last_name);
    param.set("mail",mail);
    param.set("password",password);
    param.set("lang",lang);
    api_state=API_STATE_REGISTER;
    QString surl("https://www.whatchado.com/diy/signup?");
    surl+=param.result();

    qDebug()<<surl;
    QUrl url(surl);
    QNetworkReply* reply = nam->get(QNetworkRequest(url));
    return 0;
}
int WhatchadoApi::login(QNetworkAccessManager *nam,const QString &mail,const QString &password)
{
    WhatchadoApiParam param;
    param.set("mail",mail);
    param.set("password",password);
    api_state=API_STATE_REGISTER;
    QString surl("https://www.whatchado.com/diy/login?");
    surl+=param.result();

    qDebug()<<surl;
    QUrl url(surl);
    QNetworkReply* reply = nam->get(QNetworkRequest(url));
    return 0;
}

int WhatchadoApi::logout(QNetworkAccessManager *nam,QString session)
{
    api_state=API_STATE_REGISTER;
    QString surl("https://www.whatchado.com/diy/logout?");
    surl+="hmac=";
    surl+="&session=";
    surl+=QUrl::toPercentEncoding(session);
    qDebug()<<surl;
    QUrl url(surl);
    QNetworkReply* reply = nam->get(QNetworkRequest(url));

    return 0;
}

int WhatchadoApi::uploadMeta(QNetworkAccessManager *nam,QString session,QString mail,QString lang,QString loc,QString video_type_id,QVector<int> const &index, QVector<int> const &offset_start, QVector<int> const &offset_end, QVector<int> const &vsuffix)
{
    WhatchadoApiParam param;
    param.set("session",session);
    param.set("mail",mail);
    param.set("lang",lang);
    param.set("loc",loc);
    param.set("video_type_id",video_type_id);

    QString parts;
    parts.append("[");
    int c=0;
    foreach(int i,index)
    {
        if(c) parts.append(",");
        parts.append("{\"index\":");
        parts.append(QString::number(i));
        parts.append(",\"offset_start\":");
        parts.append(QString::number(offset_start[c]));
        parts.append(",\"offset_end\":");
        parts.append(QString::number(offset_end[c]));
        parts.append(",\"suffix\":");
        parts.append(QString::number(vsuffix[c]));
        parts.append("}");
        c++;
    }
    parts.append("]");
    param.set("parts",parts);

    QString surl("https://www.whatchado.com/diy/upload_meta?");
    surl+=param.result();

    qDebug()<<surl;
    QUrl url(surl);
    QNetworkReply* reply = nam->get(QNetworkRequest(url));
    return 0;
}

int WhatchadoApi::uploadData(QNetworkAccessManager *nam, QString session, QString part_id,QString filename)
{
    WhatchadoApiParam param;
    param.set("session",session);
    param.set("part_id",part_id);

    QCryptographicHash hash(QCryptographicHash::Sha256);
    {
        QFile f(filename);
        f.open(QIODevice::ReadOnly);
        hash.addData(&f);
        f.close();
    }


    param.set("part_sha256",hash.result().toPercentEncoding());


    QString surl("https://www.whatchado.com/diy/upload_file?");
    //QString surl("http://192.168.0.56/webmail/whatchado?");
    surl+=param.result();

    //QFile *file = new QFile(filename);
    //file->open(QIODevice::ReadOnly);
    qDebug()<<surl;
    QUrl url(surl);
    QNetworkRequest request(url);


    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);


    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"video\"; filename=\"video_"+part_id+".mp4\""));
    QFile *file = new QFile(filename);
    file->open(QIODevice::ReadOnly);
    imagePart.setBodyDevice(file);
    file->setParent(multiPart);
    //imagePart.setBody("file");

    multiPart->append(imagePart);

    request.setRawHeader("Content-Type","multipart/form-data; boundary="+multiPart->boundary());
    //request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    QNetworkReply* reply = nam->post(request,multiPart);
    multiPart->setParent(reply);
    if(progressTarget)
        QObject::connect(reply, SIGNAL(uploadProgress(qint64, qint64)),progressTarget, SLOT(whatchadoapi_progress(qint64, qint64)));
    return 0;
}
