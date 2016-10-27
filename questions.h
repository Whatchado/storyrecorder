#ifndef QUESTIONS_H
#define QUESTIONS_H
#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif
#include <QString>
#include <QObject>

class questions: public QObject
{
public:
    questions();
    static void init();
    static int count(const QString interviewtype);
    static void get(const QString interviewtype,const QString lang,int i, QString &);
    static qint64 getLength(const QString &interviewtype,int i);
};

#endif // QUESTIONS_H
