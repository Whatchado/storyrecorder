#include "rules.h"
#include "ui_rules.h"

Rules::Rules(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Rules)
{
    ui->setupUi(this);
    setWindowTitle("Rule");
}

Rules::~Rules()
{
    delete ui;
}

void Rules::on_pushButton_clicked()
{
   rules = new Rules;
   rules->hide();
   this->close();
}
