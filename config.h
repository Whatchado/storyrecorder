#ifndef CONFIG_H
#define CONFIG_H
#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif
#ifdef __unix__
#define NOCAMERAINFO
//#define NOPLAYER
#endif


#define VIDEOENCODINGLOG

#endif // CONFIG_H
