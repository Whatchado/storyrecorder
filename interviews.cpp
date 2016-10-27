#include "interviews.h"
#include <QSettings>
#include <QIODevice>
#include <QObject>
Interviews interviews;

QString postotime(double p)
{
    qint64 s=p;
    QString min(QString::number(s/60));
    QString sec(QString::number(s%60));
    if(sec.length()<2) sec="0"+sec;
    //if(sec.size()<1) sec="0"+sec;
    return min+":"+sec;
}


Interviews::Interviews()
{
    QStringList paths = QStandardPaths::standardLocations(QStandardPaths::MoviesLocation);
    qDebug() << qVersion() << paths;
    if(!paths.size())
    {
        //int reply = QMessageBox::question(this, "whatchado Fehler", "Kein Video Verzeichniss gefunden!",QMessageBox::Ok);
        //QTimer::singleShot(250, qApp, SLOT(quit()));
        return;
    }
    path=paths.at(0);
    qDebug()<<path;
    if(!QDir(path).exists()) QDir().mkdir(path);
    path.append("/whatchado");
    if(!QDir(path).exists()) QDir().mkdir(path);
    printf("using video path %s\n",path.toLocal8Bit().data());
    //printf("using video path %s\n",paths.at(0).to);

}
Interviews::~Interviews()
{

}

QStringList Interviews::list()
{
    QStringList all_dirs;
    QDirIterator directories(path, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while(directories.hasNext()){
        directories.next();
        all_dirs << directories.fileName();
    }
    qDebug()<<all_dirs;
    return all_dirs;
}

QString Interviews::getTitle(const QString &id)
{
    QString fld;
    fld.append(path);
    fld.append("/");
    fld.append(id);
    fld.append("/.settings.ini");
    QSettings ini(fld,QSettings::IniFormat);
    return ini.value("title",id).toString();
}

void Interviews::setTitle(QString &id,QString title)
{
    QString fld;
    fld.append(path);
    fld.append("/");
    fld.append(id);
    fld.append("/.settings.ini");
    QSettings ini(fld,QSettings::IniFormat);
    ini.setValue("title",title);

}
QString Interviews::getUniqueName(const QString &id)
{
    QString saveid(id);
    makeFilenameSave(saveid);
    QString postfix;
    int npostfix=0;
    QString dirname;
    do {
        dirname=saveid;
        dirname+=postfix;
        postfix.clear();
        npostfix++;
        postfix.append(QString::number(npostfix));

    } while(QDir(path).exists(dirname));
    return dirname;
}
QString Interviews::getUniqueVideo(const QString &id,const QString question)
{
    QString postfix;
    int npostfix=0;
    QString dirname;
    QString filename;
    do {
        postfix.clear();
        postfix.append("_");
        postfix.append(QString::number(npostfix));
        //postfix.append""
        dirname=id;
        dirname+="/";
        filename=question;
        //dirname+="_";
        filename+=postfix;
        npostfix++;
        filename+=".mp4";
        dirname+=filename;

    } while(QDir(path).exists(dirname));
    qDebug()<<"getuniquevideo"<<path+"/"+dirname;
    // reset video settings
    setVideoLength(id,filename,0.0);
    setVideoStart(id,filename,0.0);
    setVideoStop(id,filename,0.0);
    return path+"/"+dirname;
}

QString Interviews::add(QString id)
{
    QString dirname=getUniqueName(id);
    QDir(path).mkdir(dirname);
    setTitle(dirname,id);
    return dirname;
}

QString Interviews::rename(QString &id,QString &title)
{
    QString dirname=getUniqueName(title);
    QDir(path).rename(id,dirname);
    setTitle(dirname,title);
    return dirname;
}
void Interviews::remove(QString &id,QString &video)
{
    if(!id.size()) return;
    if(!video.size()) return;
    QString dir;
    dir=path+"/"+id+"/"+video;
    //int rc=QDir(dir).remove(video);
    QFile f(dir);
    f.setPermissions(QFile::ReadOther | QFile::WriteOther);
    int rc=f.remove();
    qDebug()<<f.error();
    qDebug()<<"rc="<<rc;
}

QString Interviews::getDir(const QString &id)
{
    return path+"/"+id;
}

QStringList Interviews::getVideoTypes()
{
    QStringList types;
    types.append("standard");
    types.append("student");
    return types;
}

QString Interviews::getVideoTypeDescription(const QString &type)
{
    if(type.compare("student")==0)
        return QObject::tr("Student","videotype");
    return QObject::tr("Job","videotype");
}

void Interviews::setType(QString &id,QString &type)
{
    QString fld=getIniFile(id);
    QSettings ini(fld,QSettings::IniFormat);
    ini.setValue("type",type);
}
void Interviews::setLanguage(QString &id,QString &language)
{
    QString fld=getIniFile(id);
    QSettings ini(fld,QSettings::IniFormat);
    ini.setValue("language",language);
}
void Interviews::setFirstname(QString &id,QString &firstname)
{
    QString fld=getIniFile(id);
    QSettings ini(fld,QSettings::IniFormat);
    ini.setValue("firstname",firstname);
}

void Interviews::setQuestionsReady(QString &id,int ready)
{
    QString fld=getIniFile(id);
    QSettings ini(fld,QSettings::IniFormat);
    ini.setValue("questionsready",ready);

}
InterviewInfo Interviews::getInterviewInfo(const QString &id)
{
    InterviewInfo ret;
    qDebug()<<"id:"<<id;
    QString fld=getIniFile(id);
    QSettings ini(fld,QSettings::IniFormat);

    ret.language=ini.value("language","de").toString();
    ret.firstname=ini.value("firstname","").toString();
    ret.title=ini.value("title",id).toString();
    ret.type=ini.value("type","standard").toString();
    qDebug()<<"type="<<ret.type;
    ret.questionsready=ini.value("questionsready","0").toInt();
    return ret;
}

QString Interviews::getVideoType(QString &id)
{
    QString fld=getIniFile(id);
    QSettings ini(fld,QSettings::IniFormat);
    QString ret=ini.value("type","standard").toString();
    return ret;
}
QString Interviews::getVideoLanguage(QString &id)
{
    QString fld=getIniFile(id);
    QSettings ini(fld,QSettings::IniFormat);
    QString ret=ini.value("language","de").toString();
    return ret;
}
QString Interviews::getIniFile(const QString id)
{
    QString fld;
    fld.append(path);
    fld.append("/");
    fld.append(id);
    fld.append("/.settings.ini");
    return fld;
}

void Interviews::setConfig(QString &id,QString &key,QString &val)
{
    QString fld=getIniFile(id);
    QSettings ini(fld,QSettings::IniFormat);
    ini.setValue(key,val);
}
QString Interviews::getConfig(QString &id,QString &key)
{
    QString fld=getIniFile(id);
    QSettings ini(fld,QSettings::IniFormat);
    QString ret=ini.value(key,"").toString();
    return ret;
}

VideoInfo Interviews::getVideoInfo(const QString &id,const QString &video)
{
    VideoInfo ret;
    QString fld=getIniFile(id);
    QSettings ini(fld,QSettings::IniFormat);
    QString key="video_length/"+video;
    ret.length=ini.value(key,"").toFloat();
    key="video_start/"+video;
    ret.start=ini.value(key,"").toFloat();
    key="video_stop/"+video;
    ret.stop=ini.value(key,"").toFloat();
    return ret;
}

void Interviews::setVideoLength(QString const &id, QString &video, double length)
{
    QString fld=getIniFile(id);
    QSettings ini(fld,QSettings::IniFormat);
    QString key="video_length/"+video;
    ini.setValue(key,length);
}

void Interviews::setVideoStart(const QString &id, QString &video, double start)
{
    QString fld=getIniFile(id);
    QSettings ini(fld,QSettings::IniFormat);
    QString key="video_start/"+video;
    ini.setValue(key,start);
}

void Interviews::setVideoStop(const QString &id, QString &video, double stop)
{
    QString fld=getIniFile(id);
    QSettings ini(fld,QSettings::IniFormat);
    QString key="video_stop/"+video;
    ini.setValue(key,stop);
}

QStringList Interviews::getVideos(QString &interview,QString &question)
{
    QString fld;
    fld.append(path);
    fld.append("/");
    fld.append(interview);

    QStringList videos;
    QDirIterator files(fld, QDir::Files | QDir::NoDotAndDotDot);
    while(files.hasNext()){
        files.next();
        if(files.fileName().startsWith(question+"_"))
        {
            videos<<files.fileName();
        }
    }
    return videos;
}

void Interviews::makeFilenameSave(QString &filename)
{
    for (QChar *it=filename.begin(); it!=filename.end(); ++it) {
        if(!((*it>='A')&&(*it<='Z')))
            if(!((*it>='a')&&(*it<='z')))
                if(!((*it>='0')&&(*it<='9')))
                {
                    *it='_';
                }
    }
}

bool Interviews::removeDir(const QString &dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            QFile qf;
            QString filename(info.absoluteFilePath());
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
#ifndef __APPLE__
                filename.replace("/","\\");
#endif
                result = qf.remove(filename);
            }

            if (!result) {
                qDebug()<<"coulnd delete ("<<result<<")"<<qf.errorString() <<" "<<filename;
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }

    return result;
}
void Interviews::del(const QString &id)
{
    if(!id.size()) return;
    QString fld;
    fld.append(path);
    fld.append("/");
    fld.append(id);
    removeDir(fld);

}
