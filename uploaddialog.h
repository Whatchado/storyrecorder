#ifndef UPLOADDIALOG_H
#define UPLOADDIALOG_H
#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif
#include <QDialog>

namespace Ui {
class UploadDialog;
}

class UploadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UploadDialog(QWidget *parent = 0);
    ~UploadDialog();

private:
    Ui::UploadDialog *ui;
};

#endif // UPLOADDIALOG_H
