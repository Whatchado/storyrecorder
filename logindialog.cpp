#include "logindialog.h"
#include "ui_logindialog.h"
#include "mainwindow.h"
#include <QSettings>
#include <QCloseEvent>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    QSettings settings("whatchado", "DIY");
    QString duser(settings.value("user").toString());
    QString dpassword(settings.value("password").toString());

    ui->login_email->setText(duser);
    ui->login_password->setText(dpassword);
if(!lang.compare("de"))
    ui->lHint->setText("<a href=\"https://www.whatchado.com/de/community\">Noch kein Account?<a/>");
else
ui->lHint->setText("<a href=\"https://www.whatchado.com/en/community\">No account? <a/>");

}
QString LoginDialog::getLoginEmail()
{
    return ui->login_email->text();
}

QString LoginDialog::getLoginPassword()
{
    return ui->login_password->text();
}
QString LoginDialog::getRegFirstname()
{
    return ui->reg_firstname->text();

}
QString LoginDialog::getRegLastname()
{
    return ui->reg_lastname->text();
}
QString LoginDialog::getRegLanguage()
{
    QString ret;
    if(!ui->reg_language->currentText().compare("Deutsch"))
        ret="de";
    if(!ui->reg_language->currentText().compare("Englisch"))
        ret="en";
    if(!ui->reg_language->currentText().compare("Französisch"))
        ret="fr";
    if(!ui->reg_language->currentText().compare("Spanisch"))
        ret="es";
    if(!ui->reg_language->currentText().compare("Italienisch"))
        ret="it";

    return ret;
}
QString LoginDialog::getRegEmail()
{
    return ui->reg_email->text();

}
QString LoginDialog::getRegPassword()
{
    return ui->reg_password->text();

}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::done(int r)
{
    if(QDialog::Accepted == r)
    {
        if(ui->login_email->text().size()>0)
        {
            QDialog::done(r);
            return;
        }

        if(ui->reg_password->text().size() >= 8)
        {
            QDialog::done(r);
            return;
        }
        else
        {
            //statusBar->setText("Invalid data in text edit...try again...");
            QMessageBox::information(this, tr("whatchado Fehler"), tr("Das Passwort muss mindesten 8 Zeichen haben"),QMessageBox::Ok);
            return;
        }
    }
    else    // cancel, close or exc was pressed
    {
        QDialog::done(r);
        return;
    }
}
