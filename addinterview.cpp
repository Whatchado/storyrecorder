#include "addinterview.h"
#include "ui_addinterview.h"
#include "mainwindow.h"
#include "interviews.h"
#include <QMessageBox>
#include "rules.h"
addinterview::addinterview(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addinterview)
{
    ui->setupUi(this);

    // add interview
    connect(ui->ai_ok, SIGNAL(clicked (bool )), this, SLOT(button_ai_ok ( bool  )) );
    connect(ui->ai_cancle, SIGNAL(clicked (bool )), this, SLOT(button_ai_cancle ( bool  )) );
    //connect(mMyButton, SIGNAL(click()), this, SLOT(openNewWindow());



    ui->ai_language->addItem("Deutsch","de");
    ui->ai_language->addItem("English","en");
    //ui->ai_language->addItem("Französisch","fr");
    //ui->ai_language->addItem("Spanisch","es");
    //ui->ai_language->addItem("Italienisch","it");
    QStringList types=interviews.getVideoTypes();
    for(int a=0;a<types.size();a++)
    {
        QString desc=interviews.getVideoTypeDescription(types[a]);
        ui->ai_type->addItem(desc,types[a]);
    }
    setWindowTitle("Neues Interview");
}

addinterview::~addinterview()
{
    delete ui;
}

void addinterview::setCurrent(const QString &_id)
{
    id=_id;
    if(id.size())
    {
        InterviewInfo iinfo=interviews.getInterviewInfo(id);
        int index = ui->ai_type->findData(iinfo.type);
        if ( index != -1 ) {
            ui->ai_type->setCurrentIndex(index);
        }
        index = ui->ai_language->findData(iinfo.language);
        if ( index != -1 ) {
            ui->ai_language->setCurrentIndex(index);
        }
        ui->ai_name->setText(iinfo.title);
        ui->ai_type->setEnabled(false);
        setWindowTitle("Interview "+iinfo.title);
    }
    else
    {
        ui->ai_type->setEnabled(true);
        setWindowTitle("Neues Interview");

    }

}
QString addinterview::getCurrent()
{
    return id;
}

void addinterview::button_ai_ok(bool)
{
    if(!ui->ai_name->text().size())
    {
        QMessageBox::critical(NULL, tr("Fehler"), tr("Kein Name angegeben"));
        return;
    }

    QString type,language,title,firstname;
    type=ui->ai_type->itemData(ui->ai_type->currentIndex()).toString();
    language=ui->ai_language->itemData(ui->ai_language->currentIndex()).toString();
    title=ui->ai_name->text();
    if(id.size())
    {
        interviews.setLanguage(id,language);
        interviews.setFirstname(id,firstname);
        id=interviews.rename(id,title);
        close();
        return;
    }
    id=interviews.add(ui->ai_name->text());
    interviews.setType(id,type);
    interviews.setLanguage(id,language);

    close();

    Rules rules;
    rules.exec();

}

void addinterview::button_ai_cancle(bool)
{
    close();
}


/*void addinterview::on_ai_ok_2_clicked()
{
    Rules rules;
    rules.exec();
}*/
