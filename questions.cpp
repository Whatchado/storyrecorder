#include "questions.h"
#include <QTranslator>
#include <QString>
#include <QtGlobal>
#include <QtDebug>
#include <QObject>
#include <QCoreApplication>


static const char * const qlangs[]= {"de","en","fr","es","it"};

static const char * const qdefault[] = {
     QT_TR_NOOP("Was steht auf deiner Visitenkarte?"),
     QT_TR_NOOP("Worum geht’s in deinem Job?"),
     QT_TR_NOOP("Wie schaut dein Werdegang aus?"),
     QT_TR_NOOP("Ginge es auch ohne deinen Werdegang?"),
     QT_TR_NOOP("Was ist das Coolste an deinem Job?"),
     QT_TR_NOOP("Welche Einschränkungen bringt der Job mit sich?"),
     QT_TR_NOOP("3 Ratschläge an dein 14-jähriges Ich?")
};

static const char * const qstudent[] = {
     QT_TR_NOOP("Wer bist du und was machst du?"),
     QT_TR_NOOP("Worum geht's in deinem Studium?"),
     QT_TR_NOOP("Wie bist du zu diesem Studium gekommen?"),
     QT_TR_NOOP("Welche Voraussetzungen sind nötig?"),
     QT_TR_NOOP("Was ist das Coolste an deinem Studium?"),
     QT_TR_NOOP("Was ist die größte Herausforderung?"),
     QT_TR_NOOP("3 Dinge, die du nach deinem Studium machen möchtest?"),
     QT_TR_NOOP("Der wichtigste Ratschlag in deinem Leben?")
};

// Länge der Fragen in millisekunden
qint64 qdefault_len[]=
{
    15000,
    120000,
    120000,
    30000,
    30000,
    30000,
    30000
};
qint64 qstudent_len[]=
{
    15000,
    120000,
    120000,
    30000,
    30000,
    30000,
    30000,
    20000
};

QMap<QString,QStringList> questions_lang;
QMap<QString,QStringList> questions_lang_student;

questions::questions()
{
}
void questions::init()
{
    QTranslator trans;
    QString lpath(QCoreApplication::applicationDirPath());

    for(int a=0;a<sizeof(qlangs)/sizeof(char *);a++)
    {
        QString lang(qlangs[a]);
        bool rc=trans.load("whatchado_diy."+lang,lpath);

        qDebug()<<lpath<<"whatchado_diy."+lang<<"QObject"<<rc;
        qDebug()<<rc;

        QStringList translations;
        for(int b=0;b<sizeof(qdefault)/sizeof(char*);b++)
        {
            QString t;
            t=trans.translate("QObject ",qdefault[b]);

             if(t.isEmpty())
              translations.push_back(QString(qdefault[b]));
             else
               translations.push_back(t);
             ;
        }
        questions_lang[lang]=translations;
        translations.clear();
        for(int c=0;c<sizeof(qstudent)/sizeof(char*);c++)
        {
            QString z;
            z=trans.translate("QObject ",qstudent[c]);

             if(z.isEmpty())
               translations.push_back(QString(qstudent[c]));
             else
               translations.push_back(z);

        }
        questions_lang_student[lang]=translations;
    };


}

int questions::count(const QString interviewtype)
{
    if(!interviewtype.compare("standard"))
    {
        return sizeof(qdefault)/sizeof(const char * const);
    }
    else if(!interviewtype.compare("student"))
    {
        return sizeof(qstudent)/sizeof(const char * const);
    }
    return 0;
}

void questions::get(const QString interviewtype, const QString lang, int i, QString & ret)
{
        if(!interviewtype.compare("standard"))
        {

            if((i<0)||(i>=count(interviewtype))) return;
            QStringList q = questions_lang[lang];
            if(i<q.size()) ret.append(q[i]);
            qDebug()<<"---------------------------------------------------------";
            qDebug()<<q;

        }
        else if(!interviewtype.compare("student"))
        {
            int j=i;
            if((j<=-1)||(j>=count(interviewtype))) return;

            QStringList s = questions_lang_student[lang];
            if(j<=s.size()) ret.append(s[j]);
            qDebug()<<"---------------------------------------------------------";
            //qDebug()<<s;

        }

 return ;
}
qint64 questions::getLength(const QString &interviewtype,int i)
{
    if((i<0)||(i>=count(interviewtype))) return 0;
    if(!interviewtype.compare("standard"))
    {
        return qdefault_len[i];
    }
    else if(!interviewtype.compare("student"))
    {
        return qstudent_len[i];
    }
}
