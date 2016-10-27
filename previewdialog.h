#ifndef PREVIEWDIALOG_H
#define PREVIEWDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include "videoplaythread.h"
#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif
namespace Ui {
class PreviewDialog;
}

class PreviewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreviewDialog(QWidget *parent = 0);
    ~PreviewDialog();
    void setVideo(QString &id);
void closeEvent ( QCloseEvent * e );
public slots:
    // player
    void player_positionChanged(double position);
    void button_play(bool b);
    void button_stop(bool b);
    void button_fullscreen(bool b);
    void player_stateChanged(VideoPlayerState);
    void player_started();
    void player_stopped();
    //void currentIndexChanged(int position);
    void volumeChanged(int volume);
    void changeVolume(int volume);

    void gotoQuestion(int q);
    int  currentQuestion();
private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

    void on_button_next_clicked();

    void on_button_previous_clicked();

private:
    Ui::PreviewDialog *ui;
    //QMediaPlayer *player;
    QStringList playlist;
    QStringList videos;
    QStringList videos_start;
    QStringList videos_stop;

    int num_questions;
    QString interview;
    void nextVideo();
    bool player_running;
    bool player_force_next;
    int  player_pos;
    int player_index;
    bool player_user_stop;
    qint64 video_pos_stop;
    int position_set_for_index;

    void loadCurrentPosVideo(bool start_playing=false);
    VideoPlayThread player;
    bool loading_position;
};

#endif // PREVIEWDIALOG_H
