#include "helphelp.h"
#include "ui_helphelp.h"
#include <QAbstractButton>
HelpHelp::HelpHelp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpHelp)
{
    ui->setupUi(this);
}

HelpHelp::~HelpHelp()
{
    delete ui;
}

void HelpHelp::on_buttonBox_clicked(QAbstractButton *button)
{
    close();
}
