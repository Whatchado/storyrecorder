#include "mainwindow.h"
#include "interviews.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QLabel>
#include <QCameraViewfinder>
#include <QStandardPaths>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QTimer>
#include "listdelegate.h"
#include "helphelp.h"
#include "helpbp.h"
#include "uploaddialog.h"
#include "aboutdialog.h"
#include "previewdialog.h"
#include "qtx/qxtspanslider.h"
#include "questions.h"
#include "addinterview.h"
#include "logindialog.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include "whatchadoapi.h"
#include "videothread.h"
#include "videopreview.h"
#include <QThread>
#include <QDesktopServices>
#ifndef NOCAMERAINFO
#include <QCameraInfo>
#endif
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QAudioDeviceInfo>
#include <QSettings>
#include "welcome.h"

//#define VIDEOADD

#define RECORD_EXTRA_MS 1000
#define RECORD_EXTRA (double(RECORD_EXTRA_MS)/1000.0)

QTranslator qtTranslator;

int c_video_suffix[13]={1,2,3,4,5,6,7,1,1,1,1,1,1};
int c_video_suffix_student[13]={1,2,3,4,5,6,7,8,1,1,1,1,1};

QString lang;
QString baseurl;

void setTimeLabel(QLabel *label,double t,QString prefix="",QString postfix="")
{
    int it=t;
    //qDebug()<<it;
    if((it<0)||(it>100000000))
    {
        label->setText("-");
        return;
    }
    int ssec=(t-it)*100;
    int sec=(it)%60;
    int min=(it)/60;
    QString text(prefix);
        text+=QString::number(min);
        text+=":";
        if(sec<10)
            text+="0";
    text+=QString::number(sec);
    text+=".";
    if(ssec<10)
        text+="0";
    text+=QString::number(ssec);
text+=postfix;
    label->setText(text);
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    video_recording_name(""),
    nam(this)
{
    progressTarget=this;
    video_start_pos=0;
    video_stop_pos=0;
    player_limit_set=false;
    player_limit_stop=0;
    QSettings settings("whatchado", "whatchado_diy");
    if(settings.value("accepted").toString().compare("ok")) {
    Welcome wdialog;
    wdialog.setLang(lang);
    if(wdialog.exec()!=QDialog::Accepted) {
        QTimer::singleShot(250, qApp, SLOT(quit()));
   }}
   settings.setValue("accepted","ok");
    ui->setupUi(this);
ui->tb_help_bp->hide();

//ui->menuEinstellungen->setVisible(false);
ui->mi_quit->setVisible(false);
//ui->mi_settings->setText("Settings");
    //diskCache.setCacheDirectory(".");
   // ui->webView->page()->networkAccessManager()->setCache(&diskCache );
ui->infobox->setZoomFactor(1.25);

    // set overview page
    setOverviewWebview(lang);

    QDir dir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    if (!dir.exists()){
        bool rc= dir.mkpath(".");
        //printf("%i\n",rc);
    }



    di_pos=0;
    qDebug()<<baseurl<<"-";
    di.push_back(DownloadItem(baseurl+"de/diy/start","start_de.html"));
    di.push_back(DownloadItem(baseurl+"en/diy/start","start_en.html"));
    di.push_back(DownloadItem(baseurl+"en/diy/briefing/student/1","q_student_en_1.html"));
    di.push_back(DownloadItem(baseurl+"en/diy/briefing/student/2","q_student_en_2.html"));
    di.push_back(DownloadItem(baseurl+"en/diy/briefing/student/3","q_student_en_3.html"));
    di.push_back(DownloadItem(baseurl+"en/diy/briefing/student/4","q_student_en_4.html"));
    di.push_back(DownloadItem(baseurl+"en/diy/briefing/student/5","q_student_en_5.html"));
    di.push_back(DownloadItem(baseurl+"en/diy/briefing/student/6","q_student_en_6.html"));
    di.push_back(DownloadItem(baseurl+"en/diy/briefing/student/7","q_student_en_7.html"));
    di.push_back(DownloadItem(baseurl+"en/diy/briefing/student/8","q_student_en_8.html"));
    di.push_back(DownloadItem(baseurl+"de/diy/briefing/student/1","q_student_de_1.html"));
    di.push_back(DownloadItem(baseurl+"de/diy/briefing/student/2","q_student_de_2.html"));
    di.push_back(DownloadItem(baseurl+"de/diy/briefing/student/3","q_student_de_3.html"));
    di.push_back(DownloadItem(baseurl+"de/diy/briefing/student/4","q_student_de_4.html"));
    di.push_back(DownloadItem(baseurl+"de/diy/briefing/student/5","q_student_de_5.html"));
    di.push_back(DownloadItem(baseurl+"de/diy/briefing/student/6","q_student_de_6.html"));
    di.push_back(DownloadItem(baseurl+"de/diy/briefing/student/7","q_student_de_7.html"));
    di.push_back(DownloadItem(baseurl+"de/diy/briefing/student/8","q_student_de_8.html"));
    di.push_back(DownloadItem(baseurl+"en/diy/briefing/standard/1","q_standard_en_1.html"));
    di.push_back(DownloadItem(baseurl+"en/diy/briefing/standard/2","q_standard_en_2.html"));
    di.push_back(DownloadItem(baseurl+"en/diy/briefing/standard/3","q_standard_en_3.html"));
    di.push_back(DownloadItem(baseurl+"en/diy/briefing/standard/4","q_standard_en_4.html"));
    di.push_back(DownloadItem(baseurl+"en/diy/briefing/standard/5","q_standard_en_5.html"));
    di.push_back(DownloadItem(baseurl+"en/diy/briefing/standard/6","q_standard_en_6.html"));
    di.push_back(DownloadItem(baseurl+"en/diy/briefing/standard/7","q_standard_en_7.html"));
    di.push_back(DownloadItem(baseurl+"de/diy/briefing/standard/1","q_standard_de_1.html"));
    di.push_back(DownloadItem(baseurl+"de/diy/briefing/standard/2","q_standard_de_2.html"));
    di.push_back(DownloadItem(baseurl+"de/diy/briefing/standard/3","q_standard_de_3.html"));
    di.push_back(DownloadItem(baseurl+"de/diy/briefing/standard/4","q_standard_de_4.html"));
    di.push_back(DownloadItem(baseurl+"de/diy/briefing/standard/5","q_standard_de_5.html"));
    di.push_back(DownloadItem(baseurl+"de/diy/briefing/standard/6","q_standard_de_6.html"));
    di.push_back(DownloadItem(baseurl+"de/diy/briefing/standard/7","q_standard_de_7.html"));


    QNetworkReply* reply = nam.get(QNetworkRequest(QUrl(baseurl+"/"+lang+"/diy/start")));


    //ui->webView->setUrl(QUrl("https://www.whatchado.com/de/diy/start"));
    //apiprogress = new QProgressBar(this);
    //    apiprogress->setVisible(false);
    //    apiprogress->setMinimum(0);
    //    apiprogress->setMaximum(100);
    //    ui->statusBar->addPermanentWidget(apiprogress);
    //ai = new addinterview(this);
    //ai->hide();

    ui->video_record->hide();
    ui->video_editor->hide();

    ui->list_interviews->setAttribute(Qt::WA_MacShowFocusRect,false);
    ui->list_questions->setAttribute(Qt::WA_MacShowFocusRect,false);
    ui->list_videos->setAttribute(Qt::WA_MacShowFocusRect,false);
    ui->recordprogress->setAttribute(Qt::WA_MacShowFocusRect,false);
    ui->video_cut->setAttribute(Qt::WA_MacShowFocusRect,false);

    //QxtSpanSlider *slider=new QxtSpanSlider();
    ui->video_cut->setOrientation(Qt::Horizontal);
    ui->list_questions->hide();
    showVideoList(false);

    ui->b_preview->setEnabled(false);
    ui->b_upload->setEnabled(false);
    ui->b_delete->setEnabled(false);
    //ui->treeWidget->expandAll();
    //ui->treeWidget->setColumnHidden(1, true);
    connect(ui->list_interviews, SIGNAL(itemChanged ( QListWidgetItem *)), this, SLOT(itemChanged ( QListWidgetItem *)));
    connect(ui->list_interviews, SIGNAL(itemClicked ( QListWidgetItem *)), this, SLOT(interview_clicked ( QListWidgetItem *)));
    connect(ui->list_interviews, SIGNAL(currentRowChanged(int)), this, SLOT(interview_changed ( int)));
    connect(ui->list_questions, SIGNAL(itemClicked ( QListWidgetItem * )), this, SLOT(question_selected ( QListWidgetItem *  )) );
    connect(ui->list_videos, SIGNAL(itemClicked ( QListWidgetItem * )), this, SLOT(video_selected ( QListWidgetItem *  )) );

    ui->list_videos->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->list_videos, SIGNAL(customContextMenuRequested(const QPoint&)),this, SLOT(list_view_menu(const QPoint&)));
    ui->list_interviews->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->list_interviews, SIGNAL(customContextMenuRequested(const QPoint&)),this, SLOT(interview_view_menu(const QPoint&)));

    // video recorder
    connect(ui->video_record_start, SIGNAL(clicked(bool)), this, SLOT(video_record_start_clicked(bool)));
    //connect(ui->video_record_stop, SIGNAL(clicked(bool)), this, SLOT(video_record_stop_clicked(bool)));
    connect(ui->b_infobox_show, SIGNAL(clicked(bool)), this, SLOT(infobox_show_clicked(bool)));

    // video editor
    connect(ui->video_view_start, SIGNAL(clicked(bool)), this, SLOT(video_view_start_clicked(bool)));
    //connect(ui->video_view_stop, SIGNAL(clicked(bool)), this, SLOT(video_view_stop_clicked(bool)));
    connect(ui->b_video_select, SIGNAL(clicked(bool)), this, SLOT(video_select_clicked(bool)));
    connect(ui->b_video_delete, SIGNAL(clicked(bool)), this, SLOT(video_delete_clicked(bool)));
    connect(ui->video_cut, SIGNAL(lowerValueChanged ( int )), this, SLOT(video_cut_lowerValueChanged ( int )));
    connect(ui->video_cut, SIGNAL(upperValueChanged ( int )), this, SLOT(video_cut_upperValueChanged ( int )));
    //connect(ui->video_position, SIGNAL(valueChanged ( int )), this, SLOT(video_position_sliderMoved ( int )));

    connect(ui->video_position, SIGNAL(sliderMoved(int)), this, SLOT(video_position_sliderMoved(int)));

    // menu items
    connect(ui->mi_settings, SIGNAL(triggered (bool )), this, SLOT(menu_settings ( bool  )) );
    connect(ui->mi_help, SIGNAL(triggered (bool )), this, SLOT(menu_help ( bool  )) );
    connect(ui->mi_help_bp, SIGNAL(triggered (bool )), this, SLOT(menu_help_bp ( bool  )) );
    connect(ui->mi_quit, SIGNAL(triggered (bool )), this, SLOT(menu_quit ( bool  )) );
    connect(ui->mi_about, SIGNAL(triggered (bool )), this, SLOT(menu_about ( bool  )) );

    connect(ui->b_upload, SIGNAL(clicked (bool )), this, SLOT(button_upload ( bool  )) );
    connect(ui->b_preview, SIGNAL(clicked (bool )), this, SLOT(button_preview ( bool  )) );
    connect(ui->b_delete, SIGNAL(clicked (bool )), this, SLOT(button_delete ( bool  )) );
    connect(ui->tb_help, SIGNAL(clicked (bool )), this, SLOT(menu_help ( bool  )) );
    connect(ui->tb_help_bp, SIGNAL(clicked (bool )), this, SLOT(menu_help_bp ( bool  )) );

    connect(&nam,SIGNAL(finished(QNetworkReply *)),this,SLOT(network_finished(QNetworkReply *)));
    ui->webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect(ui->webView, SIGNAL(linkClicked (const QUrl & )), this, SLOT(webview_linkclicked(const QUrl &  )) );

    // video
    connect(&videoThread, SIGNAL(frameReady(QImage *,char *,qint64)), this, SLOT(newVideoFrame(QImage*,char *,qint64)));
    connect(&videoThread, SIGNAL(durationChanged(qint64)), this, SLOT(recorder_durationChanged(qint64)));
    connect(&videoThread, SIGNAL(encodingFrame(int,int)), this, SLOT(encodingFrame(int,int)));
    connect(&videoThread, SIGNAL(encodingDone()), this, SLOT(encodingDone()));

    ui->buttonTest->hide();
    connect(ui->buttonTest, SIGNAL(clicked ( bool)), this, SLOT(buttonTest_clicked ( bool)));

    connect(&player,SIGNAL(frameReady(QImage*,char *,double)),ui->video_view,SLOT(updateImage(QImage *,char *,double)));
    connect(&player,SIGNAL(positionChanged(double)), this, SLOT(player_positionChanged(double)));
    connect(&player,SIGNAL(durationChanged(double)), this, SLOT(player_durationChanged(double)));
    connect(&player,SIGNAL(stateChange(VideoPlayerState)),this,SLOT(player_stateChanged(VideoPlayerState)));

    connect(&tVideoPreview,SIGNAL(videoPreviewUpdate()),this,SLOT(videoPreviewUpdate()));

up_play=false;
up_pos=-1;up_pos_start=-1;up_pos_stop=-1;
    QTimer *timer = new QTimer(this);
         connect(timer, SIGNAL(timeout()), this, SLOT(updatePlayer()));
         timer->start(100);
has_video=false;
has_audio=false;
    QByteArray cameraid("/dev/video0");
    if(QCamera::availableDevices().size()>0)
    {
        has_video=true;
        cameraid=QCamera::availableDevices()[0];
    }
    QByteArray cameraDevice;
    /*
    camera=new QCamera(cameraid);
    camera->setCaptureMode(QCamera::CaptureVideo);
    mediaRecorder = new QMediaRecorder(camera);
    connect(mediaRecorder, SIGNAL(durationChanged(qint64)), this, SLOT(recorder_durationChanged(qint64)));
    connect(mediaRecorder,SIGNAL(error(QMediaRecorder::Error)),this,SLOT(recording_error(QMediaRecorder::Error)));
    camera->setViewfinder(ui->videotest);
    camera->start();
    */
    //QActionGroup *videoDevicesGroup = new QActionGroup(this);
    //videoDevicesGroup->setExclusive(true);

    updateInterviews();
    updateQuestions();
    updateVideos();
    updateInterviews();

    if(!QCamera::availableDevices().size())
    {
        //int reply = QMessageBox::question(this, "whatchado Fehler", "Kein Video Kamera gefunden!",QMessageBox::Ok);
        //QTimer::singleShot(250, qApp, SLOT(quit()));
        //return;
    }

    /*
            foreach(const QByteArray &deviceName, QCamera::availableDevices()) {
                QString description = camera->deviceDescription(deviceName);
                QAction *videoDeviceAction = new QAction(description, videoDevicesGroup);
                videoDeviceAction->setCheckable(true);
                videoDeviceAction->setData(QVariant(deviceName));
                if (cameraDevice.isEmpty()) {
                    cameraDevice = deviceName;
                    videoDeviceAction->setChecked(true);
                }
                //ui->menuDevices->addAction(videoDeviceAction);
            }
            */
    /*
    //QAudioEncoderSettings audioSettings;
    audioSettings=mediaRecorder->audioSettings();
    videoSettings=mediaRecorder->videoSettings();
    //audioSettings.setCodec("audio/vorbis");
    audioSettings.setQuality(QMultimedia::HighQuality);
    //QVideoEncoderSettings videoSettings;
    //videoSettings.setCodec("video/x-vp8");
    //videoSettings.setResolution(640,480);
    videoSettings.setQuality(QMultimedia::HighQuality);
    mediaRecorder->setEncodingSettings(audioSettings,videoSettings,"");
    //qDebug()<<"e:"<<mediaRecorder->errorString();
    qDebug()<<mediaRecorder->supportedVideoCodecs();
    //qDebug()<<"e:"<<mediaRecorder->errorString();
    */
    //player->setNotifyInterval(1000/25);

    //WhatchadoApi::registerAccount(&nam,"Florian","Auer","auer@m-otion.com","testtest","de");
    //renderer.show();
    ui->recordpreview->setScene(&scene);
    //ui->video_view->setScene(&scene);


#ifndef NOCAMERAINFO
    QList<QCameraInfo> list=QCameraInfo::availableCameras();
    if(list.size()>0)
    {
    QString defaultvideo;
    for(int a=0;a<list.size();a++)
    {
        if(list[a].description().contains("Front")) defaultvideo=list[a].description();
        if(list[a].description().contains("front")) defaultvideo=list[a].description();
    }
    if(!defaultvideo.size()) defaultvideo=list[0].description();
    videoThread.cameraname=settings.value("videodevice",defaultvideo).toString();
    settings.setValue("videodevice",videoThread.cameraname);
    has_video=true;
    }
#else
    videoThread.cameraname="/dev/video0";
#endif

    QList<QAudioDeviceInfo> lista=QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    QString defaultaudio;
    if(lista.size()>0)
    {
        defaultaudio=lista[0].deviceName();
        has_audio=true;
    }
    videoThread.audioname=settings.value("audiodevice",defaultaudio).toString();


    if((!has_video)||(!has_audio)) ui->video_record_start->setDisabled(true);

    if((!has_video)||(!has_audio))
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("whatchado StoryRecorder");

        if(!has_video)
        {
            if(!has_audio)
            {
                msgBox.setText(tr("Kein Videogerät und Audiogerät oder Mikrofon gefunden."));
                msgBox.setInformativeText(tr("Ohne Videogerät kannst du kein Video aufzeichnen, du darfst aber gern ein wenig in der App herumklicken."));
            }
            else
            {
                msgBox.setText(tr("Kein Videogerät gefunden."));
                msgBox.setInformativeText(tr("Ohne Videogerät kannst du kein Video aufzeichnen, du darfst aber gern ein wenig in der App herumklicken."));
            }
        }
        else if(!has_audio)
        {
            msgBox.setText(tr("Kein Audiogerät oder Mikrofon gefunden."));
            msgBox.setInformativeText(tr("Ohne Mikrofon kannst du kein Video aufzeichnen, du darfst aber gern ein wenig in der App herumklicken."));
        }
        QAbstractButton *myYesButton = msgBox.addButton(tr("Ja, ich will klicken!"), QMessageBox::YesRole);
        myYesButton->setFixedWidth(250);
        msgBox.setStandardButtons(QMessageBox::Cancel);
        int ret=msgBox.exec();
        qDebug()<<ret;
        switch (ret) {
          case QMessageBox::Cancel:
            QTimer::singleShot(50, qApp, SLOT(quit()));
              break;
          default:
              break;
        }
    }

    QSize s=size();
    QRect dsize=QApplication::desktop()->screenGeometry();
    if(s.width()>dsize.width())
        s.setWidth(dsize.width());
    if(s.height()>dsize.height())
        s.setHeight(dsize.height());
   resize(s);
}

void MainWindow::setOverviewWebview(QString wvlang)
{
    // set overview page
    QString filename(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    filename.append("/");
    filename.append("start_"+wvlang+".html");

    QFile f(filename);
    if (f.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&f);
        in.setCodec("UTF-8");
        QString test(in.readAll());
        ui->webView->setHtml(test,QUrl("https://www.whatchado.com/"+wvlang+"/diy/start"));
    }

}

void	MainWindow::itemChanged (QListWidgetItem * item)
{

    QString newtitle=item->data(0).toString();
    QString id=item->data(ROLE_ID).toString();
    //    qDebug()<<"rename:"<<id<<" "<<newtitle;
    QString newid=interviews.rename(id,newtitle);
    setUpdatesEnabled(false);
    updateInterviews();
    selectInterview(newid);
    setUpdatesEnabled(true);
    //QList<QTreeWidgetItem *> newitem= ui->treeWidget->findItems(newid,Qt::MatchExactly | Qt::MatchRecursive, 0);
    //if(newitem.size())
    //{
    //    ui->treeWidget->setCurrentItem(newitem[0]);
    //}
}
void MainWindow::interview_dialog(QString changeid)
{
    qDebug()<<"new_interview";
    addinterview ai;
    ai.setWindowFlags(ai.windowFlags()&(~Qt::WindowContextHelpButtonHint));
    if(!changeid.isEmpty())
        ai.setCurrent(changeid);
    ai.exec();
    setUpdatesEnabled(false);
    updateInterviews();
    updateInterviews();
    selectInterview(ai.getCurrent());
    updateQuestions();
    updateVideos();
    selectInterview(ai.getCurrent());
    setUpdatesEnabled(true);
}
void MainWindow::new_interview(bool)
{
    interview_dialog("");
}

void    MainWindow::interview_clicked(QListWidgetItem *item )
{
    qDebug()<<"interview_clicked";
    if(item->data(ROLE_DISPLAYMODE)==ROLE_DM_MENU)
    {
        interview_dialog("");
        return;
    }
}
void    MainWindow::interview_changed(int currentRow)
{
    qDebug()<<"interview_changed";
    player.stop();
    bool video_selected=false;
    if((currentRow<ui->list_interviews->count())&&(currentRow>=0))
    {
        if(ui->list_interviews->item(currentRow)->data(ROLE_DISPLAYMODE)==ROLE_DM_ITEM)
        {
            video_selected=true;
        }
        ui->b_delete->setEnabled(video_selected);
    }
    if(video_selected)
    {
        updateQuestions();
        ui->list_questions->show();
        ui->list_questions->setCurrentRow(0);
        question_selected(ui->list_questions->item(0));
        updateInterviewProgress();
    }
    else
    {
        ui->webView->show();
        ui->list_questions->hide();
        showVideoList(false);
        ui->video_editor->hide();
        ui->video_record->hide();
    }
}

void MainWindow::updateInterviewProgress()
{
    QString interview(getCurrentInterview());
    InterviewInfo iinfo= interviews.getInterviewInfo(interview);

    int numvideos=questions::count(iinfo.type);
    int progress=iinfo.questionsready;
    if(progress==0)
        ui->progressInterview->setFormat(tr("Noch keine Videos erstellt."));
    else if(progress==numvideos)
        ui->progressInterview->setFormat(tr("Alle Videos erstellt, Du kannst das Interview jetzt hochladen"));
    else
        ui->progressInterview->setFormat(""+QString::number(progress)+tr(" von ","x von x videos erstellt")+QString::number(numvideos)+tr(" Videos erstellt.","x von x videos erstellt"));
    ui->progressInterview->setValue(progress*100/numvideos);
}


void MainWindow::new_video(bool)
{
    ui->progress_encode->hide();
    ui->video_record_start->setDisabled(false);

    ui->video_record->show();
    ui->video_editor->hide();
    //ui->welcome->hide();
    ui->video_record_start->setEnabled(true);
    //ui->video_record_stop->setEnabled(false);
    QString interview=getCurrentInterview();
    QString question=getCurrentQuestion();
    InterviewInfo ii=interviews.getInterviewInfo(interview);

    recordShowInfoBox(true,ii.type,question,ii.language);

    int qid=question.mid(1).toInt();
    record_maxlen=questions::getLength(ii.type,qid);
    record_start=0;
    recorder_durationChanged(0);
    //camera->setViewfinder(ui->videotest);
    //camera->start();

    videoThread.start();
    if((!has_video)||(!has_audio)) ui->video_record_start->setDisabled(true);

    return;
}

void    MainWindow::video_view_start_clicked(bool )
{
    qDebug()<<"clicked view ispause="<<player.isPaused()<<" isplaying="<<player.isPlaying();
    if(player.isPlaying())
    {
        //ui->video_view_start->setIcon(QIcon(":/icons/resources/icon_play.svg"));
        player.pause();
    }
    else
    {
        //ui->video_view_start->setIcon(QIcon(":/icons/resources/icon_pause.svg"));
        qDebug()<<"pos="<<player.position()<<" dur="<<player.duration()<<" stop="<<player.stopPosition();
        if (
                ((player.duration()>=0)&&(player.position()>=player.duration()))||
                ((player.stopPosition()>=0)&&(player.position()>=player.stopPosition()))
                 )
        {
            player.stop();
            player.wait();
        }
        player.play();
    }

}
void    MainWindow::video_view_stop_clicked(bool)
{
    player.stop();
}
void    MainWindow::video_record_start_clicked(bool checked )
{

    //if(mediaRecorder->state()==QMediaRecorder::RecordingState)
    if(videoThread.recording)
    {
        ui->video_record_start->setIcon(QIcon(":/icons/resources/icon_record.svg"));
        video_record_stop_clicked(checked);
        return;
    }

    if(!ui->list_interviews->currentItem()) return;
    QString interview=getCurrentInterview();
    QString question = getCurrentQuestion();
    //qDebug()<<id<<" "<<interview;

    QString path=interviews.getDir(interview);
    QString url;
    video_recording_name=interviews.getUniqueVideo(interview,question);

    videoThread.setFile(video_recording_name.toLocal8Bit());

    InterviewInfo ii=interviews.getInterviewInfo(interview);
    //ui->b_infobox_show->setIcon(QIcon(":/icons/resources/icon_hide.svg"));
    ui->video_record_start->setIcon(QIcon(":/icons/resources/icon_stop.svg"));
    //recordShowInfoBox(false,ii.type,question,ii.language);
    return;
}
void    MainWindow::video_record_stop_clicked(bool)
{
    //mediaRecorder->stop();
    ui->progress_encode->show();
    ui->video_record_start->setDisabled(true);

    QTimer::singleShot(RECORD_EXTRA_MS, this , SLOT(stopRecorder()));

    return;
}

void MainWindow::stopRecorder()
{
    videoThread.stop();

}

void    MainWindow::question_selected(QListWidgetItem *)
{
    videoThread.stop();
    player.stop();

    //ui->button_newvideo->setEnabled(true);
    updateVideos();
    showVideoList(true);
    ui->webView->hide();
    ui->video_record->hide();
    ui->video_editor->hide();

    return;
}
void    MainWindow::video_selected(QListWidgetItem * item )
{
    if(item->data(ROLE_ID).toString().compare("__new_video__")==0)
    {
        new_video(true);
        showVideoList(false);
        return;
    }
    else
    {
        if(!ui->list_interviews->currentItem()) return;
        //ui->welcome->hide();

        //const Qt::ItemDataRole IdRole = Qt::ItemDataRole(Qt::UserRole + 1);
        QString interview=getCurrentInterview();
        QString question= getCurrentQuestion();
        QString video=item->data(ROLE_ID).toString();
        setupVideo(interview,question,video);

    }
    showVideoList(false);
    //if(player.isPlaying()) player.stop();
   // player_seekfirstframe=true;
    player.pause();
    player.seek(video_start_pos);
    setTimeLabel(ui->label_video_start,video_start_pos);
    setTimeLabel(ui->label_video_stop,video_stop_pos);
    //setTimeLabel(ui->videoEditTitle,video_stop_pos-video_start_pos,"Video Editor (",")");

    return;
}
void MainWindow::setupVideo(const QString &interview,const QString &question,const QString &video)
{
    // player
    //connect(&avplayer, SIGNAL(durationChanged(qint64)), this, SLOT(player_durationChanged(qint64)));
 /*   connect(avplayer, SIGNAL(positionChanged(qint64)), this, SLOT(player_positionChanged(qint64)));
    connect(avplayer, SIGNAL(stopped()), this, SLOT(player_stateChanged()));
    connect(avplayer, SIGNAL(started()), this, SLOT(player_stateChanged()));
    connect(avplayer, SIGNAL(mediaStatusChanged(QtAV::MediaStatus)), this, SLOT(player_mediaStatusChanged(QtAV::MediaStatus)));
*/
    //player.setRenderer(ui->video_view);
    //player.stop();
    //player.setFile("");
    //player.stop();
    QString path=interviews.getDir(interview);

    ui->video_record->hide();
    ui->video_editor->show();
    QString url=path;
    url+="/";
    url+=video;
    qDebug()<<"view:"<<url;
    QUrl videourl(QUrl::fromLocalFile(url));
    qDebug()<<videourl;
    qDebug()<<"xx";
    //player->setMedia(videourl);
    player.setFile(url.toLocal8Bit());
    //player->setVideoOutput(ui->video_view);
    //player.setRenderer(ui->video_view);
    ui->video_cut->blockSignals(1);
    VideoInfo vinfo=interviews.getVideoInfo(interview,video);
    if(player.duration())
      vinfo.length=player.duration()-RECORD_EXTRA;
    if(vinfo.length>0)
    {
        ui->video_cut->setMaximum(1000.0*vinfo.length);
        ui->video_cut->setLowerValue(0);
        if(!vinfo.stop)
            ui->video_cut->setUpperValue(1000.0*vinfo.length);
        else
            ui->video_cut->setUpperValue(1000.0*vinfo.stop);
        ui->video_cut->setLowerValue(1000.0*vinfo.start);
    }
    else
    {
        ui->video_cut->setLowerValue(0);
        ui->video_cut->setUpperValue(0);
    }
    ui->video_cut->blockSignals(0);
    video_start_pos = 0.001*ui->video_cut->lowerValue();
    video_stop_pos  = 0.001*ui->video_cut->upperValue();
    player_isuserpause=false;

    if(vinfo.length>0) ui->video_position->setMaximum(1000.0*vinfo.length);
//ui->video_position->setMaximum(1000);
    player.setStartPosition(0.001*ui->video_cut->lowerValue());
    player.setStopPosition(0.001*ui->video_cut->upperValue());
#ifdef NOPLAYER
    QString sysplay("ffplay ");sysplay.append(url);
    system(sysplay.toLocal8Bit());
#else
    //player.play();
    //QTimer::singleShot(200, this, SLOT(player_play()));
    qDebug()<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
#endif
    //player.playNextFrame();
    //player.pause(true);
}
void MainWindow::player_play()
{
    player_limit_set=false;
    player.play();
}

void MainWindow::menu_settings(bool)
{
    SettingsDialog settingsdialog;
    settingsdialog.setWindowFlags(settingsdialog.windowFlags()&(~Qt::WindowContextHelpButtonHint));

    if(settingsdialog.exec()==QDialog::Accepted)
    {
        QSettings settings("whatchado", "whatchado_diy");
        settings.setValue("videodevice",settingsdialog.getVideoDevice());
        settings.setValue("audiodevice",settingsdialog.getAudioDevice());
        settings.setValue("language",settingsdialog.getLanguage());
        lang=settingsdialog.getLanguage();
        videoThread.cameraname=settingsdialog.getVideoDevice();
        qApp->removeTranslator(&qtTranslator);
        qtTranslator.load("whatchado_diy."+settingsdialog.getLanguage(),QCoreApplication::applicationDirPath());
        qApp->installTranslator(&qtTranslator);
        ui->retranslateUi(this);
        setOverviewWebview(lang);
        updateInterviews();
    }

    printf("changed\n");

}
void MainWindow::menu_quit(bool)
{
    qDebug()<<"quit";
    close();
}
void MainWindow::menu_about(bool)
{
/*    layout()->invalidate();
    layout()->activate();
    return;*/
    AboutDialog dialog;
    dialog.exec();
}
void MainWindow::menu_help(bool)
{
    QDesktopServices::openUrl ( QUrl(tr("https://www.whatchado.com/de/misc/faq")) );
return;
    qDebug()<<"help";
    HelpHelp dialog;
    dialog.exec();
}
void MainWindow::menu_help_bp(bool)
{
    QDesktopServices::openUrl ( QUrl(tr("https://www.whatchado.com/de/tutorial/diy")) );
return;
    qDebug()<<"help bp";
    HelpBP dialog;
    dialog.exec();
}
void MainWindow::button_upload(bool)
{
    regi_user="";
    regi_password="";
    //UploadDialog dialog;
    //dialog.exec();
    LoginDialog dialog;
    dialog.setWindowFlags(dialog.windowFlags()&(~Qt::WindowContextHelpButtonHint));

    if(dialog.exec()==QDialog::Accepted)
    {
        upload_state=0;
        QString loginEmail=dialog.getLoginEmail();
        if(!loginEmail.size())
        {
            QString regFirstname(dialog.getRegFirstname());
            QString regLastname(dialog.getRegLastname());
            QString regEmail(dialog.getRegEmail());
            QString regLang(dialog.getRegLanguage());
            QString regPassword(dialog.getRegPassword());
             regi_user=regEmail;
             regi_password=regPassword;
            api.registerAccount(&nam,regFirstname,regLastname,regEmail,regPassword,regLang);
        }
        else
        {
            QString regEmail(dialog.getLoginEmail());
            QString regPassword(dialog.getLoginPassword());
            QSettings settings("whatchado", "DIY");
            settings.setValue("user",regEmail);
            settings.setValue("password",regPassword);
            api.login(&nam,regEmail,regPassword);
        }
        uploadprogressdialog.setModal(true);
        uploadprogressdialog.exec();
    }
    else
    {
       //QMessageBox::question(this, "whatchado Fehler", "Login Fehlgeschlagen!",QMessageBox::Ok);
    }
}
void MainWindow::button_preview(bool)
{
    QString id=getCurrentInterview();
    if(!id.size()) return;

    PreviewDialog dialog;
    dialog.setWindowFlags(dialog.windowFlags()&(~Qt::WindowContextHelpButtonHint));

    dialog.setVideo(id);
    dialog.exec();
}
void MainWindow::button_delete(bool)
{
    QString id = getCurrentInterview();
    if(!id.size()) return;

    QMessageBox msgBox;
    QPushButton *connectButton = msgBox.addButton(tr("Ja"), QMessageBox::ActionRole);
    msgBox.addButton(tr("Nein"),QMessageBox::ActionRole);
    msgBox.setText(tr("Wollen Sie Das Interview wirklich löschen?"));
    msgBox.setInformativeText("");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.exec();

    if (msgBox.clickedButton() == connectButton)
    {
        qDebug()<<"deleting "<<id;
        setUpdatesEnabled(false);
        player.stop();
        player.wait();
        interviews.del(id);
        updateInterviews();
        ui->list_questions->hide();
        showVideoList(false);
        ui->video_editor->hide();
        ui->video_record->hide();
        ui->webView->show();
        ui->list_interviews->setCurrentRow(-1);
        updateQuestions();
        updateVideos();
        setUpdatesEnabled(true);

    }

}


void MainWindow::recording_error(QMediaRecorder::Error)
{
    printf("error\n");
    return;
}

void MainWindow::updateInterviews()
{
    qDebug()<<"before"<<ui->list_interviews->count();
    QString current=getCurrentInterview();
    QVariant style(1);
    if(ui->list_interviews->count())
    {

        while (ui->list_interviews->count()>0) {
            QListWidgetItem *i=ui->list_interviews->takeItem(0);
            if(i) delete i;
        }
    }
    //else
    {
        ui->list_interviews->setItemDelegate(new ListDelegate(ui->list_interviews));
        ui->list_interviews->clear();

        // group items
        QListWidgetItem *baseitem=new QListWidgetItem();
        baseitem->setFlags(Qt::ItemIsEnabled);
        baseitem->setText(tr("Interviews"));
        baseitem->setData(ROLE_STYLE,style);
        QVariant dmgroup(ROLE_DM_GROUP);
        baseitem->setData(ROLE_DISPLAYMODE, dmgroup);
        ui->list_interviews->addItem(baseitem);

        QListWidgetItem *menuitem=new QListWidgetItem();
        menuitem->setText(tr("Neues Interview..."));
        menuitem->setFlags(Qt::ItemIsEnabled);
        menuitem->setData(ROLE_STYLE,style);
        QVariant dmm(ROLE_DM_MENU);
        menuitem->setData(ROLE_DISPLAYMODE, dmm);
        menuitem->setData(ROLE_ID, "new");
        menuitem->setData(ROLE_ICON, ":/icons/resources/plussymbol-white.svg");

        ui->list_interviews->addItem(menuitem);

        QPushButton *newinterview=new QPushButton(this);
        ui->list_interviews->setItemWidget(menuitem,newinterview);
        QIcon niicon(":/icons/resources/plussymbol-white.svg");
        newinterview->setIcon(niicon);
        newinterview->setText(tr("Neues Interview..."));
        newinterview->setStyleSheet("margin:6px;text-align:left;");
        connect(newinterview, SIGNAL(clicked( bool )), this, SLOT(new_interview ( bool  )) );
    }

    QStringList all_dirs;
    all_dirs=interviews.list();
    for(int a=0;a<all_dirs.size();a++)
    {
        QListWidgetItem *item = new QListWidgetItem();

        InterviewInfo info=interviews.getInterviewInfo(all_dirs.at(a));
        int nquestions=questions::count(info.type);
        QString desc2;
        desc2+=QString::number(info.questionsready)+'/'+QString::number(nquestions);


        QString id=all_dirs.at(a);
        //QString title=interviews.getTitle(all_dirs.at(a));
        QVariant dm(ROLE_DM_ITEM);
        //QString type=interviews.getVideoType(id);
        QString ititle;
        ititle.append(info.firstname);
        if(ititle.size()) ititle.append(" ");
        ititle.append(info.title);
        item->setText(ititle);
        item->setData(ROLE_DISPLAYMODE, dm);
        item->setData(ROLE_ID,id);
        item->setData(ROLE_STYLE,style);
        item->setData(ROLE_DESCRIPTION,interviews.getVideoTypeDescription(info.type));
        item->setData(ROLE_DESCRIPTION2,desc2);
        item->setFlags(Qt::ItemIsSelectable |Qt::ItemIsEditable|Qt::ItemIsEnabled);
        QString iconname;
        iconname+=":/icons/resources/logo_interview_"+info.type;
        if(info.questionsready==nquestions)
            iconname+="_done";
        iconname+=".svg";
        item->setData(ROLE_ICON,iconname);

        ui->list_interviews->addItem(item);

    }
}
void MainWindow::updateQuestions(bool force)
{
    static QString lastinterview("");
    QString interview=getCurrentInterview();

    if(!force)
        if(!lastinterview.compare(interview))
            return;

    lastinterview=interview;

    QString current=getCurrentQuestion();
    QVariant style(2);
    if(!ui->list_interviews->currentItem()) return;
    //QString interview = ui->list_interviews->currentItem()->data(ROLE_ID).toString();
    //QString itype=interviews.getVideoType(interview);
    InterviewInfo info=interviews.getInterviewInfo(interview);
    ui->list_questions->clear();
    ui->list_questions->setItemDelegate(new ListDelegate(ui->list_questions));

    int n=questions::count(info.type);
    int numready=0;
    for(int a=0;a<n;a++)
    {
        QString title,id;
        id.append("q");
        id.append(QString::number(a));
        questions::get(info.type,info.language,a,title);
        QStringList videos=interviews.getVideos(interview,id);
        QString desc,desc2;
        desc2=QString::number(a+1);
        if(videos.size()==1)
            desc=QString::number(videos.size())+tr(" Aufnahme");
        else
            desc=QString::number(videos.size())+tr(" Aufnahmen");
        QListWidgetItem *item = new QListWidgetItem();
        //item->setText(questions::get(itype,a));
        item->setText(title);
        item->setData(ROLE_DESCRIPTION, desc);
        item->setData(ROLE_DESCRIPTION2, desc2);
        item->setData(ROLE_ID, id);
        item->setData(ROLE_STYLE,style);

        item->setFlags(Qt::ItemIsSelectable |Qt::ItemIsEnabled);
        if(videos.size())
            item->setData(ROLE_ICON,":/icons/resources/movie_done_active.svg");
        else
            item->setData(ROLE_ICON,":/icons/resources/movie_unfinished_unactive.svg");
        QVariant dm(ROLE_DM_ITEM);
        item->setData(ROLE_DISPLAYMODE, dm);

        ui->list_questions->addItem(item);
        if(videos.size())
            numready++;

    }
    interviews.setQuestionsReady(interview,numready);
    ui->b_preview->setEnabled(numready>0);
    ui->b_upload->setEnabled(numready==n);

}

void MainWindow::updateVideos()
{
    //camera->setViewfinder(ui->videotest);

    ui->list_videos->clear();
    ui->list_videos->setItemDelegate(new ListDelegate(ui->list_videos));

    QString interview=getCurrentInterview();
    if(!interview.size()) return;

    QString id = getCurrentQuestion();

    QString path=interviews.getDir(interview);

    QStringList videos=interviews.getVideos(interview,id);

    QString selkey="selected/"+id;
    QString selected_video=interviews.getConfig(interview,selkey);

    if((!selected_video.size())&&(videos.size()))
    {
        selected_video=videos[0];
    }
    bool hasSelected=false;
    QListWidgetItem *item0=NULL;
    for(int a=0;a<videos.size();a++)
    {
        QListWidgetItem *item = new QListWidgetItem();
        if(a==0) item0=item;
        item->setText("Video");
        item->setData(ROLE_ID, videos[a]);
        item->setFlags(Qt::ItemIsSelectable |Qt::ItemIsEnabled);
        item->setIcon(QIcon(":/icons/icon_frage.png"));
        QString filename;
        filename.append(path);
        filename.append("/");
        filename.append(videos[a]);
        item->setData(Qt::DisplayRole, filename);
        QString desc;
        VideoInfo vinfo=interviews.getVideoInfo(interview,videos[a]);
        double showlength=vinfo.length;
        if(vinfo.stop)
            showlength=vinfo.stop-vinfo.start;
        desc.append(postotime(showlength));
        item->setData(ROLE_DESCRIPTION, desc);
        QVariant dm(ROLE_DM_ITEM);
        item->setData(ROLE_DISPLAYMODE, dm);
        item->setData(ROLE_ICON,":/icons/icon_video.svg");
        if(videos[a].compare(selected_video)==0)
        {
            item->setData(ROLE_ICON2,":/icons/icon_selectedvideo.svg");
            hasSelected=true;
        }
        ui->list_videos->addItem(item);
    }
if((!hasSelected)&&(videos.size()))
{
        interviews.setConfig(interview,selkey,videos[0]);
        if(item0) item0->setData(ROLE_ICON2,":/icons/icon_selectedvideo.svg");

}
    QListWidgetItem *item = new QListWidgetItem();
    item->setText("Video");
    item->setData(ROLE_ID, "");
    item->setFlags(Qt::ItemIsSelectable |Qt::ItemIsEnabled);
    item->setIcon(QIcon(":/icons/icon_frage.png"));

    item->setData(Qt::DisplayRole, tr("Neue Antwort aufnehmen"));
    item->setData(ROLE_ID, "__new_video__");
    item->setData(ROLE_DESCRIPTION,tr( "Aufnehmen"));
    item->setData(ROLE_ICON,":/icons/resources/plussymbol-orange.svg");
    QVariant dm(ROLE_DM_MENU);
    item->setData(ROLE_DISPLAYMODE, dm);

    ui->list_videos->addItem(item);
    /*QVideoWidget *addvideobutton=new QVideoWidget(this);
            camera->setViewfinder(addvideobutton);
            camera->start();
            //ui->centralWidget->addWidget(addvideobutton);
            ui->list_videos->setItemWidget(item,addvideobutton);*/
#ifdef VIDEOADD
    QVideoWidget *addvideobutton=new QVideoWidget(this);
    QSize s;
    s.setWidth(640);
    s.setHeight(480);
    addvideobutton->resize(s);
    camera->setViewfinder(addvideobutton);
    if(camera->state()!=QCamera::ActiveState) camera->start();
    ui->list_videos->setItemWidget(item,addvideobutton);
#endif

}

void MainWindow::selectInterview(const QString &id)
{
    qDebug()<<"select interview"<<id;
    for(int a=0;a<ui->list_interviews->count();a++)
    {
        if(ui->list_interviews->item(a)-> data(ROLE_ID).toString().compare(id)==0)
        {
            qDebug()<<"found";
            ui->list_interviews->item(a)->setSelected(true);
            ui->list_interviews->setCurrentItem(ui->list_interviews->item(a));
            interview_clicked(ui->list_interviews->item(a));
        }
    }

}
void MainWindow::selectVideo(const QString &id)
{
    if(ui->list_videos->count()>0)ui->list_videos->setCurrentItem(ui->list_videos->item(0));
    for(int a=0;a<ui->list_videos->count();a++)
    {
        if(ui->list_videos->item(a)->data(ROLE_ID).toString().compare(id)==0)
        {
            ui->list_videos->item(a)->setSelected(true);
            ui->list_videos->setCurrentItem(ui->list_videos->item(a));        }
    }
}
void MainWindow::selectQuestion(const QString &id)
{
    for(int a=0;a<ui->list_questions->count();a++)
    {
        if(ui->list_questions->item(a)->data(ROLE_ID).toString().compare(id)==0)
        {
            ui->list_questions->item(a)->setSelected(true);
            ui->list_questions->setCurrentItem(ui->list_questions->item(a));
        }
    }
}
QString MainWindow::getCurrentInterview()
{
    if(!ui->list_interviews->currentItem()) return "";
    QString interview=ui->list_interviews->currentItem()->data(ROLE_ID).toString();
    return interview;
}

QString MainWindow::getCurrentQuestion()
{
    if(!ui->list_questions->currentItem()) return "";
    QString id = ui->list_questions->currentItem()->data(ROLE_ID).toString();
    return id;
}

QString MainWindow::getCurrentVideo()
{
    if(!ui->list_videos->currentItem()) return "";
    QString id = ui->list_videos->currentItem()->data(ROLE_ID).toString();
    return id;
}


void MainWindow::video_select_clicked(bool)
{

    player.stop();
    qDebug()<<"video selected";
    QString interview=getCurrentInterview();
    QString question = getCurrentQuestion();
    QString video = getCurrentVideo();
    if((!interview.size())||(!question.size())||(!video.size())) return;

    setUpdatesEnabled(false);
    QString selkey="selected/"+question;
    interviews.setConfig(interview,selkey,video);
    updateVideos();
    ui->video_record->hide();
    ui->video_editor->hide();
    showVideoList(true);
    setUpdatesEnabled(true);

}

void MainWindow::video_delete_clicked(bool)
{
    QMessageBox msgBox;
    QPushButton *connectButton = msgBox.addButton(tr("Ja"), QMessageBox::ActionRole);
    msgBox.addButton(tr("Nein"),QMessageBox::ActionRole);
    msgBox.setText(tr("Wollen Sie das Video wirklich löschen?"));
    msgBox.setInformativeText("");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.exec();

    if (msgBox.clickedButton() == connectButton) {
    setUpdatesEnabled(false);
    player.stop();

        ui->video_record->hide();
        ui->video_editor->hide();
        showVideoList(true);
        QString interview=getCurrentInterview();
        QString question = getCurrentQuestion();
        QString video = getCurrentVideo();
        if((interview.size())&&(video.size())) {
        qDebug()<<"remove "<<interview<<" "<<video;
        interviews.remove(interview,video);
        updateVideos();
        updateQuestions(true);
        updateInterviews();
        selectInterview(interview);
        selectQuestion(question);
        updateVideos();
        selectVideo("");
        ui->list_videos->setCurrentItem(ui->list_videos->item(0));
        video_select_clicked(true);
        }
            setUpdatesEnabled(true);
    }
    clearVideoFrameCache();
}

void MainWindow::recorder_durationChanged(qint64 duration)
{
    QString text;
    text.append(QString::number(duration/1000));
    if(!lang.compare("en"))
        text.append(" sec of max ");
      else
    text.append(" sec von max ");
    text.append(QString::number(record_maxlen/1000));
    text.append(" sec");
    ui->recordprogress->setFormat(text);
    ui->recordprogress->setMaximum(record_maxlen);
    ui->recordprogress->setValue(duration);
    QString def="QProgressBar {border: none;text-align: center;}";
    //QString danger = "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #FF0350,stop: 0.4999 #FF0020,stop: 0.5 #FF0019,stop: 1 #FF0000 );}";
    QString danger= "QProgressBar {border: none;;text-align: center;} QProgressBar::chunk {background:#FF0350;}";
    QString warn= "QProgressBar {border: none;text-align: center;} QProgressBar::chunk {background:#FF8350;}";
    if(record_maxlen<duration)
    {
        video_record_stop_clicked(true);
                return;
    }

    if(record_maxlen-duration<5000)
        ui->recordprogress->setStyleSheet(danger);
    else if(record_maxlen-duration<10000)
        ui->recordprogress->setStyleSheet(warn);
    else
        ui->recordprogress->setStyleSheet(def);
}
// player
void MainWindow::player_durationChanged(double duration)
{
    if(duration>=1.0)
    {
        QString interview=getCurrentInterview();
        QString video = getCurrentVideo();
        interviews.setVideoLength(interview,video,duration);
        ui->video_cut->setMaximum((duration-RECORD_EXTRA)*1000);
ui->video_position->setMaximum((duration-RECORD_EXTRA)*1000);
        if((video_stop_pos==0)||(video_stop_pos>=duration-RECORD_EXTRA))
        {
            interviews.setVideoStop(interview,video,duration-RECORD_EXTRA);
            video_stop_pos=duration-RECORD_EXTRA;
            player.setStopPosition(duration-RECORD_EXTRA);
            ui->video_cut->blockSignals(1);
            ui->video_cut->setUpperValue(1000.0*(duration-RECORD_EXTRA));
            ui->video_cut->blockSignals(0);
            setTimeLabel(ui->label_video_stop,video_stop_pos);
            setTimeLabel(ui->label_video_stop,video_stop_pos);

        }
    }
    return;
}

void MainWindow::player_positionChanged(double position)
{
    //qDebug()<<"qp="<<position;
    qint64 pos=position*1000;
    if(pos<ui->video_cut->lowerValue()+(1000/25)) pos=ui->video_cut->lowerValue();
    if(pos>ui->video_cut->upperValue()-(1000/25)) pos=ui->video_cut->upperValue();
    bool bs=ui->video_position->blockSignals(true);
    ui->video_position->setValue(pos);
    ui->video_position->blockSignals(bs);
    setTimeLabel(ui->label_video_pos,double(pos)/1000);
    return;
}
void MainWindow::player_stateChanged(VideoPlayerState state)
{
    qDebug()<<"state change: "<<state;
    if(state==VIDEOPLAYER_PLAY)
    {
        ui->video_view_start->setIcon(QIcon(":/icons/resources/icon_pause.svg"));
    }
    else
    {
        ui->video_view_start->setIcon(QIcon(":/icons/resources/icon_play.svg"));
    }
}
/*
    void MainWindow::player_mediaStatusChanged(QtAV::MediaStatus status)
    {
        //EndOfMedia
        qDebug()<<"--------------------------status"<<status;
        if(player.isPlaying())
        {
            ui->video_view_start->setIcon(QIcon(":/icons/resources/icon_pause.svg"));
        }
        else
        {
            ui->video_view_start->setIcon(QIcon(":/icons/resources/icon_play.svg"));
            if(!player.isPaused())
            {
                ui->video_position->setValue(ui->video_cut->upperValue());

            }
        }

    }
*/
void MainWindow::video_cut_lowerValueChanged ( int value)
{
    //player.stop();
    qDebug()<<"lower changed:"<<value;
    QString interview=getCurrentInterview();
    QString video = getCurrentVideo();
    interviews.setVideoStart(interview,video,0.001*value);
    video_start_pos=0.001*value;
    //player.setStartPosition(value);
    up_pos=0.001*value;
    up_pos_start=0.001*value;

    player.setStartPosition(0.001*value);
    player.seek(0.001*value);
    //player.play();
    player_limit_set=true;
    player_limit_stop=0.001*value+2.000;
    setTimeLabel(ui->label_video_start,video_start_pos);
    setTimeLabel(ui->label_video_stop,video_stop_pos);
    //setTimeLabel(ui->videoEditTitle,video_stop_pos-video_start_pos,"Video Editor (",")");

}

void MainWindow::video_cut_upperValueChanged ( int value)
{
    player_limit_set=false;
    qDebug()<<"upperchanged:"<<value;
    QString interview=getCurrentInterview();
    QString video = getCurrentVideo();
    interviews.setVideoStop(interview,video,0.001*value);
    video_stop_pos=0.001*value;
    player.setStopPosition(0.001*value);
    double spos=0.001*value-2.0;
    if(spos<0) spos=0;
    qDebug()<<" play="<<player.isPlaying()<<" pause="<<player.isPaused()<<" stop="<<player.isStopped();
    if(!player.isPlaying())
    {
        player.seek(0.001*value);
        player.pause();
    }
    else
    {
        player.seek(spos);
    }
    //player.play();

//player.seek(0.001*value-2.0);
//player.play();

    setTimeLabel(ui->label_video_start,video_start_pos);
    setTimeLabel(ui->label_video_stop,video_stop_pos);
    //setTimeLabel(ui->videoEditTitle,video_stop_pos-video_start_pos,"Video Editor (",")");
}
void MainWindow::video_position_sliderMoved ( int value )
{

    qDebug()<<"video_position_sliderMoved:"<<value;
    //player.setPosition(value);
    up_play=true;
    up_pos=0.001*value;
    //player.playNextFrame();
}

void MainWindow::recordShowInfoBox(bool show,const QString &type,const QString &question,const QString &lang)
{
    //qDebug()<<"-------------_"<<type<<"-"<<question<<"-"<<lang;
    //http://www.whatchado.com/en/diy/briefing/standard/1
    if(!show)
    {
        ui->infobox->hide();
        return;
    }
    ui->infobox->show();
    QString url;
    int qid=question.mid(1).toInt();
    //https://www.whatchado.com/api/helptext/de/standard/1
    //url="http://www.whatchado.com/api/helptext/"+lang+"/"+type+"/"+QString::number(qid+1);
    url="http://www.whatchado.com/"+lang+"/diy/briefing/"+type+"/"+QString::number(qid+1);
    //ui->infobox->setUrl(QUrl(url));
    QString filename(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    filename.append("/");
    filename.append("q_");
    filename.append(type);
    filename.append("_");
    filename.append(lang);
    filename.append("_");
    filename.append(QString::number(qid+1));
    filename.append(".html");
    QFile f(filename);
    if (f.open(QFile::ReadOnly | QFile::Text))
    {
    QTextStream in(&f);
    //qDebug() << f.size() << in.readAll();
    QString test(in.readAll());
    ui->infobox->setHtml(test,QUrl(url));
    }

}
void    MainWindow::infobox_show_clicked(bool)
{
    QString interview=getCurrentInterview();
    QString question=getCurrentQuestion();
    InterviewInfo ii=interviews.getInterviewInfo(interview);
    if(ui->infobox->isVisible())
    {
        ui->b_infobox_show->setIcon(QIcon(":/icons/resources/icon_hide.svg"));
        recordShowInfoBox(false,ii.type,question,ii.language);
    }
    else
    {
        ui->b_infobox_show->setIcon(QIcon(":/icons/resources/icon_show.svg"));
        recordShowInfoBox(true,ii.type,question,ii.language);
    }
}
void MainWindow::network_finished(QNetworkReply * reply)
{
    qDebug()<<"network finished-----------------------------------------------------------------------------------";
    QVariant statusCodeV =reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    //QVariant redirectionTargetUrl =reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    // see CS001432 on how to handle this
    if(di_pos>=0) {

        if (reply->error() == QNetworkReply::NoError)
        {
            QByteArray bytes = reply->readAll();  // bytes
            QString data(bytes); // string
            QFile file(di[di_pos].file);
            file.open(QIODevice::WriteOnly);
            //QDataStream out(&file);
            //out << data.toUtf8();
            //out << bytes;
            file.write(bytes);
        }
        di_pos++;
        if(di_pos<di.size())
        {
            QNetworkReply* reply = nam.get(QNetworkRequest(QUrl(di[di_pos].url)));

        }
        else
            di_pos=-1;
        return;
    };
    // no error received?
    if (reply->error() == QNetworkReply::NoError)
    {

        QByteArray bytes = reply->readAll();  // bytes
        QString data(bytes); // string
        qDebug()<<data;
        QJsonDocument d = QJsonDocument::fromJson(data.toUtf8());
        QJsonObject sett2 = d.object();
        QJsonValue value = sett2.value(QString("status"));
        QString status(value.toString());
        if(!status.compare("error"))
        {
            QString message(sett2.value(QString("message")).toString());
            qDebug()<<message;
            //            abort();
            uploadprogressdialog.close();
            QMessageBox::question(this,tr( "whatchado Fehler"), message,QMessageBox::Ok);
        }
        else
        {
            if(regi_user.size()) {
            QSettings settings("whatchado", "DIY");
            settings.setValue("user",regi_user);
            settings.setValue("password",regi_password);
            }
            QString interview(getCurrentInterview());
            InterviewInfo iinfo= interviews.getInterviewInfo(interview);
            int numvideos=questions::count(iinfo.type);
            if(upload_state==0)
            {
                QString interview(getCurrentInterview());
                upload_session=sett2.value(QString("session")).toString();
                qDebug()<<"session:"<<upload_session;
                upload_state=1;
                upload_files.clear();
                upload_index.clear();
                upload_start.clear();
                upload_end.clear();
                upload_suffix.clear();
                for(int a=0;a<numvideos;a++)
                {
                    QString question="q"+QString::number(a);
                    QString selkey="selected/"+question;
                    QString sel=interviews.getConfig(interview,selkey);
                    if(sel.isEmpty())
                    {
                        QStringList v=interviews.getVideos(interview,question);
                        if(v.size())
                            sel=v[0];

                    }
                    qDebug()<<"sel:"<<sel;
                    VideoInfo vi=interviews.getVideoInfo(interview,sel);
                    upload_files.append(sel);
                    upload_index.append(a+1);
                    upload_start.append(vi.start*1000);
                    upload_end.append(vi.stop*1000);

                    if(!interviews.getVideoType(interview).compare("student"))
                      upload_suffix.append(c_video_suffix_student[a]);
                    else
                        upload_suffix.append(c_video_suffix[a]);

                }
                qDebug()<<"index:"<<upload_files;
                qDebug()<<"index:"<<upload_index;
                qDebug()<<"index:"<<upload_start;
                qDebug()<<"index:"<<upload_end;
                qDebug()<<"index:"<<upload_suffix;
                QString type="1";
                if(!interviews.getVideoType(interview).compare("student"))
                    type="9";
                api.uploadMeta(&nam,upload_session,"",iinfo.language,"at",type,upload_index,upload_start,upload_end,upload_suffix);
            }
            else if((upload_state>=1)&&(upload_state<=numvideos))
            {
                if(upload_state==1) {
                    upload_partid.clear();
                    QVariant vdata(sett2.value("part_ids").toVariant());
                    QList<QVariant> parts( vdata.toList() );
                    for(int a=0;a<numvideos;a++)
                    {
                        QMap<QString,QVariant> part(parts[a].toMap());
                        upload_partid.append(part["part_id"].toString());
                    }
                    qDebug()<<"parts:"<<upload_partid;
                }
                QString i(getCurrentInterview());

                QString filename(interviews.getDir(i));
                filename.append("/");
                //if(!interviews.getVideoType(i).compare("student"))
                    filename.append(upload_files[upload_state-1]);
                qDebug()<<filename;
                api.uploadData(&nam,upload_session,upload_partid[upload_state-1],filename);
                upload_state++;
            }
            else
            {
                uploadprogressdialog.close();
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setWindowTitle("whatchado StoryRecorder");
                msgBox.setText(tr("Upload erfolgreich!"));
                msgBox.setInformativeText(tr("Gib Deiner Story den letzten Schliff und teile sie mit der Welt!"));
                QAbstractButton *myYesButton = msgBox.addButton(tr("Weiter zu meiner Story"), QMessageBox::AcceptRole);
                myYesButton->setFixedWidth(250);
                    myYesButton->setStyleSheet("min-width:300px;width:300px;");
                msgBox.setWindowFlags(msgBox.windowFlags() & (~(Qt::WindowCloseButtonHint|Qt::WindowSystemMenuHint)));
                //msgBox.setStandardButtons(QMessageBox::Cancel);
                int ret=msgBox.exec();

                QDesktopServices::openUrl ( QUrl(tr("https://www.whatchado.com/de/users/stories")) );

            }
        }
    }
    else
    {
        // handle errors here
        qDebug()<<"http error";
        QString errmsg(tr("Netzwerk Kommunkationsfehler"));
        errmsg.append(" ");
        errmsg.append(reply->errorString());
        QMessageBox::question(this, tr("whatchado Fehler"), errmsg,QMessageBox::Ok);
        uploadprogressdialog.close();
    }

    delete reply;
}
void    MainWindow::list_view_menu(const QPoint&pos)
{
    QPoint globalPos = ui->list_videos->mapToGlobal(pos);

    QMenu myMenu;
    QAction *aSelect =myMenu.addAction(tr("Video auswählen"));
    QAction *aDelete =myMenu.addAction(tr("Video löschen"));

    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem==aSelect)
    {
        video_select_clicked(true);
    }
    else if (selectedItem==aDelete)
    {
        video_delete_clicked(true);
    }
}
void    MainWindow::interview_view_menu(const QPoint&pos)
{

    QPoint globalPos = ui->list_interviews->mapToGlobal(pos);
    QString interview=getCurrentInterview();
    if(interview.isEmpty()) return;
    if(interview.compare("new")==0) return;
    if(!ui->list_interviews->itemAt(pos)) return;
    QMenu myMenu;
    myMenu.addAction(tr("Bearbeiten..."));
    if(ui->b_preview->isEnabled())
      myMenu.addAction(tr("Preview..."));
    myMenu.addAction(tr("Löschen"));

    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem)
    {
        //connect(ui->b_preview, SIGNAL(clicked (bool )), this, SLOT(button_preview ( bool  )) );
        //connect(ui->b_delete, SIGNAL(clicked (bool )), this, SLOT(button_delete ( bool  )) );
        if(!selectedItem->text().compare(tr("Preview...")))
        {
            button_preview(true);
        }
        else if(!selectedItem->text().compare(tr("Löschen")))
        {
            button_delete(true);
        }
        else
        {
            interview_dialog(interview);
        }
    }
}
void MainWindow::webview_linkclicked(const QUrl &url)
{
    if(!url.fragment().compare("addinterview"))
    {
        interview_dialog("");
    }
}

void MainWindow::newVideoFrame(QImage *img,char *imgdata,qint64 duration)
{
    //qDebug()<<"result="<<img->size().height();
    static QGraphicsPixmapItem *item=NULL;
    static QImage *lastimg=NULL;
    static char *lastimgdata=NULL;
    //if(lastimg) printf("imgp2:%p\n",lastimg->data_ptr());
    if(lastimg) delete lastimg;
    if(lastimgdata) free(lastimgdata);
    lastimg=img;
    lastimgdata=imgdata;
    if(item) scene.removeItem(item);
    if(item) delete item;
    item = new QGraphicsPixmapItem(QPixmap::fromImage(*img));
    scene.addItem(item);
    ui->recordpreview->fitInView(scene.sceneRect(),Qt::KeepAspectRatio);
}
void MainWindow::newVideoPlayerFrame(QImage *img,qint64 duration)
{
/*
    qDebug()<<"result="<<img->size().height();
    static QGraphicsPixmapItem *item=NULL;
    static QImage *lastimg=NULL;
    //if(lastimg) printf("imgp2:%p\n",lastimg->data_ptr());
    if(lastimg) delete lastimg;
    lastimg=img;
    if(item) scene.removeItem(item);
    if(item) delete item;
    item = new QGraphicsPixmapItem(QPixmap::fromImage(*img));
    scene.addItem(item);
    ui->video_view->fitInView(scene.sceneRect(),Qt::KeepAspectRatio);
    player_positionChanged(duration*1000);
    */
}




MainWindow::~MainWindow()
{
    videoThread.stop();
    player.stop();
    videoThread.wait();
    player.wait();

    //QMediaContent d;
    //player->setMedia(d);
    //if(player) delete player;
    //delete mediaRecorder;
    //mediaRecorder=NULL;
    delete ui;
}
void    MainWindow::buttonTest_clicked(bool )
{
    player.seek(1000);
    bool before=player.isPlaying();
    if(!player.isPlaying()) player.play();
    qDebug()<<":::::::::::::::::::::::::::::::"<<before<<"---"<<player.isPlaying();
}
void    MainWindow::updatePlayer( )
{
    if(up_play)
    {
        if(!player.isPlaying()) player.play();
    up_play=false;
    }
    if(up_pos_start!=-1)
    {
    player.setStartPosition(up_pos_start);
    up_pos_start=-1;
    }
    if(up_pos_stop!=-1)
    {
    player.setStopPosition(up_pos_stop);
    up_pos_stop=-1;
    }
    if(up_pos!=-1)
    {
    player.seek(up_pos);
    up_pos=-1;
    }

}

void MainWindow::encodingFrame(int current,int n)
{
    ui->progress_encode->setRange(0,n);
    ui->progress_encode->setValue(n-current);
    int progress=0;
    if(n) progress=100.0*double(n-current)/n;
 ui->progress_encode->setFormat(tr("Speichere Video ")+QString::number(progress)+"%");
}

void MainWindow::encodingDone()
{
    qDebug()<<"encoding done";
    setUpdatesEnabled(false);
    QString interview=getCurrentInterview();
    QString question = getCurrentQuestion();
    QString video = getCurrentVideo();

    ui->video_record->hide();
    ui->video_editor->show();
    showVideoList(false);

    QFileInfo qd(video_recording_name);
    QString fn(qd.fileName());
    qDebug()<<"fi:"<<interview<<"fn:"<<fn;
    //interviews.setVideoStart(interview,fn,1);
    //interviews.setVideoStop(interview,fn,2);
    //interviews.setVideoLength(interview,fn,3);
    interviews.setVideoStart(interview,fn,0);
    //interviews.setVideoStop(interview,fn,mediaRecorder->duration());
    //interviews.setVideoLength(interview,fn,mediaRecorder->duration());

    updateQuestions(true);
    updateInterviews();

    selectInterview(interview);
    selectQuestion(question);
    updateVideos();

    selectVideo(fn);
    //camera->setViewfinder(ui->videotest);

    setupVideo(interview,question,fn);

    ui->video_record->hide();
    ui->video_editor->show();
    showVideoList(false);

    player.pause();
    setUpdatesEnabled(true);
}


void MainWindow::on_video_view_start_clicked()
{

}
void MainWindow::showVideoList(bool vlshow)
{
if(vlshow)
{
    ui->list_videos->show();
    ui->progressInterview->show();
    ui->b_upload->show();
}
else
{
    ui->list_videos->hide();
    ui->progressInterview->hide();
    ui->b_upload->hide();

}
}

void MainWindow::whatchadoapi_progress(qint64 i,qint64 n)
{
    double sprogress=double(i)*100.0/n;
    if(upload_state<2)
        sprogress=0;
    else
    {
        sprogress=(sprogress/7.0)+(100.0/7.0)*(upload_state-2);
    }
    uploadprogressdialog.setProgress((int)sprogress);
    //qDebug()<<"progress:"<<i<<"/"<<n<<" P="<<(sprogress);
    //apiprogress->setFormat("Interview Upload "+QString::number((int)sprogress)+"%");
}


void MainWindow::videoPreviewUpdate()
{
    ui->list_videos->update();
}

void MainWindow::on_tb_home_clicked()
{
    ui->webView->show();
    ui->list_questions->hide();
    showVideoList(false);
    ui->video_editor->hide();
    ui->video_record->hide();
    ui->list_interviews->setCurrentRow(-1);

}

void MainWindow::on_b_video_back_clicked()
{
    player.stop();
    showVideoList(true);
    ui->video_editor->hide();
    ui->video_record->hide();
}
