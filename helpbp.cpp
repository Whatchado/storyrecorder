#include "helpbp.h"
#include "ui_helpbp.h"

HelpBP::HelpBP(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpBP)
{
    ui->setupUi(this);
}

HelpBP::~HelpBP()
{
    delete ui;
}
