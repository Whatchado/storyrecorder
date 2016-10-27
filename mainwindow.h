#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif
#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QCamera>
#include <QStandardPaths>
#include <QMediaService>
#include <QMediaRecorder>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QNetworkAccessManager>
#include <QGraphicsScene>
#include <QProgressBar>
#include <QTranslator>
#include "videothread.h"
#include "videoplaythread.h"
#include "uploadthread.h"
#include "uploadprogressdialog.h"

extern QTranslator qtTranslator;
extern int c_video_suffix[13];
extern int c_video_suffix_student[13];
class addinterview;

class DownloadItem {
public:
    DownloadItem(QString u,QString f) {
        url=u;
        file.clear();
        file.append(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
        file.append("/");
        file.append(f);
    };
    QString url;
    QString file;
};

extern QString lang;
extern QString baseurl;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    VideoThread videoThread;
    VideoPlayThread player;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setOverviewWebview(QString wvlang);

    void updateInterviews();
    void updateQuestions(bool force=false);
    void updateInterviewProgress();
    void updateVideos();
    void new_video(bool checked);
    void selectInterview(const QString &id);
    void selectQuestion(const QString &id);
    void selectVideo(const QString &id);
    void setupVideo(const QString &interview,const QString &question,const QString &video);
    void recordShowInfoBox(bool show,const QString &type,const QString &question ,const QString &lang);
    void interview_dialog(QString changeid);

public slots:
    void new_interview(bool);

    void    video_record_start_clicked(bool );
    void    video_record_stop_clicked(bool );
    void    video_view_start_clicked(bool);
    void    video_view_stop_clicked(bool );
    void    question_selected(QListWidgetItem * item );
    void    video_selected(QListWidgetItem * item );
    void    recording_error(QMediaRecorder::Error error);
    void    infobox_show_clicked(bool );
    void    buttonTest_clicked(bool );
    void    updatePlayer( );
    void    stopRecorder( );

    // interview list
    void	itemChanged ( QListWidgetItem * item);
    void    interview_clicked(QListWidgetItem *item);
    void    interview_changed(int currentRow);
    void    list_view_menu(const QPoint&pos);
    void    interview_view_menu(const QPoint&pos);
    void    video_select_clicked(bool );
    void    video_delete_clicked(bool );

    // player
    void player_durationChanged(double duration);
    void player_positionChanged(double position);
    void player_stateChanged(VideoPlayerState);
    void player_play(void);
    void recorder_durationChanged(qint64 position);

    void encodingFrame(int current,int n);
    void encodingDone();

    void video_cut_lowerValueChanged ( int value);
    void video_cut_upperValueChanged ( int value);
    void video_position_sliderMoved ( int value );
    //menu
    void menu_settings(bool);
    void menu_quit(bool);
    void menu_about(bool);
    void menu_help(bool);
    void menu_help_bp(bool);

    // toolbar
    void button_upload(bool);
    void button_preview(bool);
    void button_delete(bool);

    // network
    void network_finished(QNetworkReply*);

    void webview_linkclicked(const QUrl &url);
//video
    void newVideoFrame(QImage *,char *,qint64);
    void newVideoPlayerFrame(QImage *,qint64);

    void whatchadoapi_progress(qint64 i,qint64 n);

    void videoPreviewUpdate();
private slots:
    void on_video_view_start_clicked();

    void on_tb_home_clicked();

    void on_b_video_back_clicked();

private:
    //QCamera * camera;
    //QMediaRecorder* mediaRecorder;

    Ui::MainWindow *ui;

    UploadProgressDialog uploadprogressdialog;

    QString path;

    //QMediaPlayer *player;
    bool player_seekfirstframe;
    bool player_isuserpause;
    bool player_limit_set;
    double player_limit_stop;

    qint64 record_maxlen;
    qint64 record_start;

    //QAudioEncoderSettings audioSettings;
    //QVideoEncoderSettings videoSettings;
    //QString videoContainerFormat;

    QString getCurrentInterview();
    QString getCurrentQuestion();
    QString getCurrentVideo();
    void showVideoList(bool vlshow=true);
    double video_start_pos;
    double video_stop_pos;
    bool video_position_setup;
    QString video_recording_name;

    //addinterview *ai;
    QNetworkAccessManager nam;

    QGraphicsScene scene;

    bool up_play;
    double up_pos,up_pos_start,up_pos_stop;

    int upload_state;
    QString upload_session;

    QVector<QString> upload_files;
    QVector<QString> upload_partid;
    QVector<int> upload_index;
    QVector<int> upload_start;
    QVector<int> upload_end;
    QVector<int> upload_suffix;

    bool has_video,has_audio;

    QList<DownloadItem> di;
    int di_pos;

    QString regi_user,regi_password;
};

#endif // MAINWINDOW_H
