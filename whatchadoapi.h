#ifndef WHATCHADOAPI_H
#define WHATCHADOAPI_H
#include <QNetworkAccessManager>
#include <QJsonDocument>

#define API_STATE_NONE     0
#define API_STATE_LOGIN    1
#define API_STATE_REGISTER 2

#define APIVERSION "1.1"


extern int api_state;
extern QObject *progressTarget;
class WhatchadoApi
{
    //Q_OBJECT
public:
    WhatchadoApi() {};
    ~WhatchadoApi() {};
    //static int api_state;
    int registerAccount(QNetworkAccessManager *nam,QString first_name,QString last_name,QString mail,QString password,QString lang);
    int login(QNetworkAccessManager *nam,const QString &mail,const QString &password);
    int logout(QNetworkAccessManager *nam,QString session);
    int uploadMeta(QNetworkAccessManager *nam, QString session, QString mail, QString lang, QString loc, QString video_type_id, QVector<int> const &index, QVector<int> const &offset_start, QVector<int> const &offset_end, QVector<int> const &vsuffix);
    int uploadData(QNetworkAccessManager *nam, QString session, QString part_id,QString filename);

};

extern WhatchadoApi api;

#endif // WHATCHADOAPI_H
