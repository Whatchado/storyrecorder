#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H
#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif
#include "config.h"
#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    QString getAudioDevice();
    QString getVideoDevice();
    QString getLanguage();
private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
