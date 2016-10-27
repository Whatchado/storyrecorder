#ifndef LOG_H
#define LOG_H
#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif
#include <QDebug>

//#define LOG_VIDEOPLAYER
//#define LOG_VIDEODECODE
//#define LOG_VIDEOBUFFER

inline void log(const char *module, const char* format, ... )
{
    while(1) {
#ifdef LOG_VIDEODECODE
        if(!strcmp(module,"VIDEODECODE")) break;
#endif;
#ifdef LOG_VIDEOPLAYER
        if(!strcmp(module,"VIDEOPLAYER")) break;
#endif;
#ifdef LOG_VIDEOBUFFER
        if(!strcmp(module,"VIDEOBUFFER")) break;
#endif;
        if(module[0]=='!') break;
        return;
    }
    va_list arglist;

#if defined(__unix__)||defined(__APPLE__)
    char *logline=NULL;
    va_start( arglist, format );
    vasprintf(&logline, format, arglist );
    va_end( arglist );
#else
    char logline[4096]="";
    va_start( arglist, format );
    vsnprintf(logline,sizeof(logline), format, arglist );
    va_end( arglist );
#endif
    if(logline)
    {
        qDebug()<<module<<": "<<logline;
#if defined(__unix__)||defined(__APPLE__)
        free(logline);
#endif
    }
    else
        qDebug()<<module<<": (no logline)";

}

#endif // LOG_H
