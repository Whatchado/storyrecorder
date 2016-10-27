#ifndef WELCOME_H
#define WELCOME_H

#include <QDialog>

namespace Ui {
class Welcome;
}

class Welcome : public QDialog
{
    Q_OBJECT

public:
    explicit Welcome(QWidget *parent = 0);
    ~Welcome();
void setLang(QString &lang);
private slots:
    void on_pushButton_clicked();

private:
    Ui::Welcome *ui;
};

#endif // WELCOME_H
