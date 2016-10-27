#include "previewdialog.h"
#include "ui_previewdialog.h"
#include "interviews.h"
#include "questions.h"
#include <QTime>

int video_sort[]={6,0,4,5,1,2,3,7,8,9,10,11,12};
int video_sort_student[]={6,0,5,4,1,2,7,3,8,9,10,11,12};

PreviewDialog::PreviewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreviewDialog),
    videos(),
    videos_start(),
    videos_stop()
{
    loading_position=false;
    num_questions=0;
    player_user_stop=false;
    position_set_for_index=-1;
    ui->setupUi(this);
    // player
    connect(ui->button_play, SIGNAL(clicked(bool)), this, SLOT(button_play(bool)) );
    //connect(ui->button_stop, SIGNAL(clicked(bool)), this, SLOT(button_stop(bool)) );

    connect(&player,SIGNAL(frameReady(QImage*,char *,double)),ui->video_preview,SLOT(updateImage(QImage *,char *,double)));
    //connect(&player,SIGNAL(positionChanged(double)), this, SLOT(player_positionChanged(double)));
    //connect(&player,SIGNAL(durationChanged(double)), this, SLOT(player_durationChanged(double)));
    connect(&player,SIGNAL(stateChange(VideoPlayerState)),this,SLOT(player_stateChanged(VideoPlayerState)));

    player_force_next=false;
    //ui->volume->setValue(player->volume());

    //avplayer.setRenderer(ui->video_preview);
player_index=0;
QString surl(QCoreApplication::applicationDirPath()+"/DIY_DE_STANDARD_INTRO.mp4");
player.setFile(surl.toLocal8Bit().data());
//player.pause();
QImage *bg=new QImage(":/resources/preview.png");
//player.updateImage(bg,"",0);
QMetaObject::invokeMethod( ui->video_preview, "updateImage", Q_ARG( QImage *,bg ), Q_ARG( char *,strdup("") ), Q_ARG( double,0 ) );
player.stop();
ui->button_play->setIcon(QIcon(":/icons/resources/icon_play.svg"));
ui->button_play->setText("");
    return;
}
void PreviewDialog::loadCurrentPosVideo(bool start_playing)
{
    qDebug()<<start_playing<<"-----------------------------";
    if(player_index<0) {return;};
    if(player_index>=videos.length()) {return;};

    qDebug()<<"video:"<<videos[player_index];
    qDebug()<<"video_start:"<<videos_start[player_index];
    qDebug()<<"video_stop:"<<videos_stop[player_index];
    if(player.isRunning()) {
    player.stop();
    player.waitTillStopped();
    }
    player.setFile(videos[player_index].toStdString().data());

    double start=videos_start[player_index].toDouble();
    double stop=videos_stop[player_index].toDouble();
    qDebug()<<"video_start:"<<start;
    qDebug()<<"video_stop:"<<stop;

    //if(start_playing) avplayer.play();
    if(start>0)
    {
        player.setStartPosition(start);
        qDebug()<<start;
    }
    if(stop>0)
    {
        player.setStopPosition(stop);
        qDebug()<<stop;
    }
if(start_playing)
    player.start();
}

void PreviewDialog::volumeChanged(int volume)
{
    //if(volume!=ui->volume->value())
    //{
//        ui->volume->setValue(volume);
  //  }
}
void PreviewDialog::changeVolume(int volume)
{
    //avplayer.setVolume(volume);
}

void PreviewDialog::button_play(bool b)
{
    player_running=true;
    player_pos=-1;
    if(player.isPlaying())
    {
        ui->button_play->setText("");
        ui->button_play->setIcon(QIcon(":/icons/resources/icon_play.svg"));
        player.pause();
    }
    else
    {
        player_user_stop=false;
        ui->button_play->setText("");
        ui->button_play->setIcon(QIcon(":/icons/resources/icon_pause.svg"));

        if(player_index==0)
        {
            loadCurrentPosVideo(true);
        }
        else
            player.play();
    }
qDebug()<<"play";
    return;
}
void PreviewDialog::button_stop(bool b)
{
    return;
    player_user_stop=true;
    player_running=false;
    //avplayer.stop();
    ui->button_play->setText("Play");
    ui->button_play->setIcon(QIcon(":/icons/resources/icon_play.svg"));

}
void PreviewDialog::button_fullscreen(bool b)
{
    qDebug()<<"fullscreen";
    //ui->video_preview->setFullScreen(true);
    return;
}
void PreviewDialog::setVideo(QString &id)
{
    qDebug()<<"v"<<videos;
    videos.clear();
    videos_start.clear();
    videos_stop.clear();
    QString itype=interviews.getVideoType(id);
QString lang=interviews.getVideoLanguage(id);
    num_questions=questions::count(itype);

    //QString surl(QCoreApplication::applicationDirPath()+"/DIY_"+lang.toUpper()+"_STANDARD_INTRO.mp4");
    QString surl(QCoreApplication::applicationDirPath()+"/DIY_DE_STANDARD_INTRO.mp4");
    videos<<surl;
    videos_start<<QString::number(-1);
    videos_stop<<QString::number(-1);

    for(int a=0;a<num_questions;a++)
    {
        QString surl(QCoreApplication::applicationDirPath()+"/DIY_"+lang.toUpper()+"_"+itype.toUpper()+"_0"+QString::number(video_sort[a]+1)+".mp4");
        videos<<surl;
        videos_start<<QString::number(-1);
        videos_stop<<QString::number(-1);

        QString question("q"+QString::number(video_sort[a]));
        QStringList v=interviews.getVideos(id,question);
        qDebug()<<"v="<<v;
        if(v.size())
        {
            QString sel=v[0];
            QString selkey="selected/"+question;
            QString selected_video=interviews.getConfig(id,selkey);

            for(int b=0;b<v.size();b++)
            {
                if(!v[b].compare(selected_video))
                    sel=v[b];
            }
            qDebug()<<"sel:"<<sel;
            QString path=interviews.getDir(id);

            QString vurl=path;
            vurl+="/";
            vurl+=sel;
            videos<<vurl;
            VideoInfo vinfo=interviews.getVideoInfo(id,sel);
            videos_start<<QString::number(vinfo.start);
            videos_stop<<QString::number(vinfo.stop);
            //qDebug()<<"view:"<<url;
        }

    }
player_index=0;
    loadCurrentPosVideo();

}


void PreviewDialog::player_positionChanged(double position)
{

}
void PreviewDialog::player_stateChanged(VideoPlayerState state)
{
    if(loading_position) return;
  if(state==VIDEOPLAYER_STOP)
      player_stopped();
}
void PreviewDialog::player_started()
{
}
void PreviewDialog::player_stopped()
{

    if(loading_position) return;

    if(!player_user_stop)
    {
    player_index++;
    if(player_index>=videos.length())
       {
        ui->button_play->setIcon(QIcon(":/icons/resources/icon_play.svg"));
        player_index=0;

    }
    else
    {
        loading_position=true;
        loadCurrentPosVideo(true);
        loading_position=false;
        //avplayer.play();
    }
    }
}
PreviewDialog::~PreviewDialog()
{
    delete ui;
}
void PreviewDialog::closeEvent ( QCloseEvent * e )
{
    player.stop();
    //player.waitTillStopped();
    player.wait();
    QThread::msleep(100);
    qDebug()<<"close";
}

void PreviewDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    player.stop();
    player.wait();
    QThread::msleep(100);
}

void PreviewDialog::gotoQuestion(int q)
{
    player.disable_state_changesignals=true;
    player.stop();

player_index=1+q*2;
qDebug()<<"player_index:"<<player_index;
loadCurrentPosVideo(true);
//QThread::usleep(1000000);
player.disable_state_changesignals=false;
}
int  PreviewDialog::currentQuestion()
{
    int ret=(player_index-1)/2;
    if(ret<0) ret=0;
    if(ret>=num_questions) ret=num_questions-1;
qDebug()<<"c:"<<ret;
return ret;
}

void PreviewDialog::on_button_next_clicked()
{
    int c=currentQuestion();
if(c<num_questions-1)
      gotoQuestion(c+1);
}

void PreviewDialog::on_button_previous_clicked()
{
    int c=currentQuestion();
    if(c>0)
    gotoQuestion(c-1);
}
