#ifndef UPLOADPROGRESSDIALOG_H
#define UPLOADPROGRESSDIALOG_H
#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif
#include <QDialog>

namespace Ui {
class UploadProgressDialog;
}

class UploadProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UploadProgressDialog(QWidget *parent = 0);
    ~UploadProgressDialog();
    void setProgress(double p);
private:
    Ui::UploadProgressDialog *ui;
};

#endif // UPLOADPROGRESSDIALOG_H
