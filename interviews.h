#ifndef INTERVIEWS_H
#define INTERVIEWS_H
#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif
#include <QStringList>
#include <QDir>
#include <QDirIterator>
#include <QStandardPaths>
#include <QDebug>

class VideoInfo {
public:
    double length;
    double start;
    double stop;
};

class InterviewInfo {
public:
    QString title;
    QString firstname;
QString type;
QString language;
int questionsready;
};

QString postotime(double p);

class Interviews
{
public:
    Interviews();
    ~Interviews();

    QStringList list();
    QString getTitle(const QString &id);
    void setTitle(QString &id,QString title);
    QString getUniqueName(const QString &id);
    QString getUniqueVideo(const QString &id,const QString question);
    QString add(QString id);
    QString getDir(const QString &id);
    QString rename(QString &id,QString &title);
    void del(const QString &id);

    // videos
    QStringList getVideos(QString &interview,QString &question);
    void    remove(QString &id,QString &video);
    QStringList getVideoTypes();
    QString getVideoTypeDescription(const QString &type);
    void setType(QString &id,QString &type);
    void setLanguage(QString &id,QString &language);
    void setFirstname(QString &id,QString &firstname);
    void setQuestionsReady(QString &id,int ready);
    QString getVideoType(QString &id);
    QString getVideoLanguage(QString &id);
    InterviewInfo getInterviewInfo(const QString &id);
    VideoInfo getVideoInfo(const QString &id,const QString &video);
    void setVideoLength(QString const &id,QString &video,double length);
    void setVideoStart(QString const &id,QString &video,double start);
    void setVideoStop(QString const &id,QString &video,double stop);

    void setConfig(QString &id,QString &key,QString &val);
    QString getConfig(QString &id,QString &key);
private:
    QString path;

    QString getIniFile(const QString id);
    void makeFilenameSave(QString &filename);
    bool removeDir(const QString &dirName);
};

extern Interviews interviews;
#endif // INTERVIEWS_H
