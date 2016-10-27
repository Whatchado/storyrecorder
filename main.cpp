#include "mainwindow.h"
#include "settingsdialog.h"
#include <QApplication>
#include <QSettings>
#include <stdio.h>
#include <stdlib.h>
#include <QFile>
#include <QDebug>
#include <qglobal.h>
#include <QTranslator>
//#include <QCameraInfo>
#include "questions.h"

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    //print_stacktrace();
    if(msg.contains("QFSFileEngine::open: No file name specified")) return;
if(msg.contains("couldn't create image from  \"\"")) return;
    switch (type) {
    case QtDebugMsg:
        if(!strcmp(qPrintable(msg),"1")) abort();
        fprintf(stderr, "Debug: %s\n", qPrintable(msg));
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s \n", qPrintable(msg));
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s\n", qPrintable(msg));
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s\n", qPrintable(msg));
        //abort();
    }
}

int main(int argc, char *argv[])
{
#ifdef __unix__
    setbuf(stdout,NULL);
#endif
    #ifdef __unix__
    //qSetMessagePattern("%{file}(%{line}): %{message}\n - %{backtrace} * %{backtrace DEPTH=10}");
    qInstallMessageHandler(customMessageHandler);
#endif
    //setbuf(stdout, NULL);
    av_register_all();
    avdevice_register_all();
    QApplication a(argc, argv);
    //QString lang(QLocale::system().name());
QSettings reg("HKEY_CURRENT_USER\\Software\\whatchado StoryRecorder",QSettings::NativeFormat);
QString installlang(reg.value("Installer Language").toString());
qDebug()<<installlang;
if(!installlang.compare("1033"))
{
    installlang="en";
}
else if(!installlang.compare("1033"))
{
    installlang="de";
}/*
else if(!installlang.compare("1036"))
{
    installlang="fr";
}
else if(!installlang.compare("1034"))
{
    installlang="es";
}
else
{
    installlang="it";
}*/
QSettings settings("whatchado", "whatchado_diy");
lang=settings.value("language",lang).toString();
baseurl=settings.value("baseurl","https://www.whatchado.com/").toString();
QString lpath(QCoreApplication::applicationDirPath());
bool rc=qtTranslator.load("whatchado_diy."+lang,lpath);
a.installTranslator(&qtTranslator);
    // Load an application style
    QFile styleFile( ":/styles/resources/style.qss" );
    styleFile.open( QFile::ReadOnly );

    // Apply the loaded stylesheet
    QString style( styleFile.readAll() );
    a.setStyleSheet( style );

  questions::init();

    MainWindow w;
    w.show();


    return a.exec();
}
