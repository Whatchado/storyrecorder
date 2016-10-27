#-------------------------------------------------
#
# Project created by QtCreator 2014-02-25T13:01:38
#
#-------------------------------------------------

QT       += core gui webkitwidgets multimediawidgets svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = whatchado_StoryRecorder
TEMPLATE = app

images.path = $$OUT_PWD
images.files += resources/cutscenes/*.mp4
INSTALLS += images

ICON = resources/osx_icon.icns
RC_FILE = whatchado_diy.rc

LIBS += -L"C:\ffmpeg\lib"
LIBS += -lavformat -lavutil -lavcodec -lavdevice -lswscale -lswresample
LIBS += -L/usr/local/lib -L/opt/local/lib


DEFINES += __STDC_CONSTANT_MACROS
DEFINES += __STDC_FORMAT_MACROS

INCLUDEPATH += /usr/local/include /opt/local/include /usr/local/ffmpeg/include
INCLUDEPATH += "C:\ffmpeg\include"

TRANSLATIONS += languages/whatchado_diy.de.ts\
                languages/whatchado_diy.en.ts\
                languages/whatchado_diy.fr.ts\
                languages/whatchado_diy.es.ts\
                languages/whatchado_diy.it.ts

QMAKE_CXXFLAGS_WARN_OFF = -Wunused-parameter
QMAKE_CXXFLAGS_WARN_OFF = -Wunused-variable
!isEmpty(TRANSLATIONS) {

isEmpty(QMAKE_LRELEASE) {
win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

isEmpty(TS_DIR):TS_DIR = Translations

TSQM.name = lrelease ${QMAKE_FILE_IN}
TSQM.input = TRANSLATIONS
TSQM.output = $$TS_DIR/${QMAKE_FILE_BASE}.qm
TSQM.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN}
TSQM.CONFIG = no_link
QMAKE_EXTRA_COMPILERS += TSQM
PRE_TARGETDEPS += compiler_TSQM_make_all
} else:message(No translation files in project)

SOURCES += main.cpp\
        mainwindow.cpp \
    settingsdialog.cpp \
    interviews.cpp \
    listdelegate.cpp \
    helphelp.cpp \
    helpbp.cpp \
    uploaddialog.cpp \
    previewdialog.cpp \
    qtx/qxtspanslider.cpp \
    qtx/qxtglobal.cpp \
    questions.cpp \
    addinterview.cpp \
    startview.cpp \
    logindialog.cpp \
    whatchadoapi.cpp \
    videopreview.cpp \
    videothread.cpp \
    videoencode.cpp \
    video_image.cpp \
    videoencodethread.cpp \
    videoplaythread.cpp \
    audiobuffer.cpp \
    videobuffer.cpp \
    videoview.cpp \
    videodecodethread.cpp \
    uploadthread.cpp \
    uploadprogressdialog.cpp \
    aboutdialog.cpp \
    welcome.cpp \
    rules.cpp

HEADERS  += mainwindow.h \
    settingsdialog.h \
    interviews.h \
    listdelegate.h \
    helphelp.h \
    helpbp.h \
    uploaddialog.h \
    previewdialog.h \
    qtx/qxtspanslider_p.h \
    qtx/qxtspanslider.h \
    qtx/qxtglobal.h \
    questions.h \
    addinterview.h \
    startview.h \
    logindialog.h \
    whatchadoapi.h \
    videopreview.h \
    videothread.h \
    videoencode.h \
    config.h \
    video_image.h \
    videoencodethread.h \
    videoplaythread.h \
    audiobuffer.h \
    videobuffer.h \
    videoview.h \
    videodecodethread.h \
    log.h \
    uploadthread.h \
    uploadprogressdialog.h \
    aboutdialog.h \
    welcome.h \
    rules.h

FORMS    += mainwindow.ui \
    settingsdialog.ui \
    helphelp.ui \
    helpbp.ui \
    uploaddialog.ui \
    previewdialog.ui \
    mainwindow_test.ui \
    addinterview.ui \
    startview.ui \
    logindialog.ui \
    uploadprogressdialog.ui \
    aboutdialog.ui \
    welcome.ui \
    rules.ui

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    whatchado_diy.rc
