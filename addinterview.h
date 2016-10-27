#ifndef ADDINTERVIEW_H
#define ADDINTERVIEW_H
#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif
#include <QDialog>
#include "rules.h"

namespace Ui {
class addinterview;
}

class addinterview : public QDialog
{
    Q_OBJECT

public:
    explicit addinterview(QWidget *parent = 0);
    void setCurrent(const QString &_id);
    QString getCurrent();
    ~addinterview();
public slots:
    // add interview
    void button_ai_ok(bool);
    void button_ai_cancle(bool);
    //void on_ai_ok_2_clicked();
    //void openNewWindow();

private:
    Ui::addinterview *ui;
    addinterview *addInterview;
    Rules *rules;


    //Ui::addinterview*mMyNewWindow;
    QString id;
    QString language;
};

#endif // ADDINTERVIEW_H
