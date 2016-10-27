#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H
#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif
#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

    QString getLoginEmail();
    QString getLoginPassword();
    QString getRegFirstname();
    QString getRegLastname();
    QString getRegLanguage();
    QString getRegEmail();
    QString getRegPassword();

    void done(int r);

private slots:
//    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
