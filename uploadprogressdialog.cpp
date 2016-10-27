#include "uploadprogressdialog.h"
#include "ui_uploadprogressdialog.h"

UploadProgressDialog::UploadProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UploadProgressDialog)
{
    ui->setupUi(this);
}

UploadProgressDialog::~UploadProgressDialog()
{
    delete ui;
}
void UploadProgressDialog::setProgress(double p)
{
    ui->uploadProgress->setValue(p);
}
