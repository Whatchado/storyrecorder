#include "config.h"
#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QCamera>
#ifndef NOCAMERAINFO
#include <QCameraInfo>
#endif
#include <QAudioDeviceInfo>
#include <QMediaRecorder>
#include <QSettings>
SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);


#ifndef NOCAMERAINFO
    foreach(const QCameraInfo &device, QCameraInfo::availableCameras()) {
        ui->cb_settings_camera->addItem(device.description());
    }
#endif
    foreach(const QAudioDeviceInfo &device, QAudioDeviceInfo::availableDevices(QAudio::AudioInput)) {
        ui->cb_settings_audio->addItem(device.deviceName());
    }
    ui->cb_settings_language->addItem("Deutsch","de");
    ui->cb_settings_language->addItem("Englisch","en");
    //ui->cb_settings_language->addItem("Französisch","fr");
    //ui->cb_settings_language->addItem("Spanisch","en");
   //ui->cb_settings_language->addItem("Italienisch","it");

    QSettings settings("whatchado", "whatchado_diy");

    ui->cb_settings_camera->setCurrentText(settings.value("videodevice").toString());
    ui->cb_settings_audio->setCurrentText(settings.value("audiodevice").toString());

    QString clang(settings.value("language").toString());
    int index = ui->cb_settings_language->findData(clang);
    if ( index != -1 ) {
        ui->cb_settings_language->setCurrentIndex(index);
    }


}
QString SettingsDialog::getAudioDevice()
{
    QString r(ui->cb_settings_audio->currentText());
    return r;
}

QString SettingsDialog::getVideoDevice()
{
    QString r(ui->cb_settings_camera->currentText());
    return r;
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
QString SettingsDialog::getLanguage()
{
    QString language;
    language=ui->cb_settings_language->itemData(ui->cb_settings_language->currentIndex()).toString();

    return language;
}
