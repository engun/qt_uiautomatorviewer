#include "settingdialog.h"
#include "ui_settingdialog.h"
#include "utils/uisetting.h"
#include "utils/adb.h"
#include <QFileDialog>

SettingDialog::SettingDialog(QWidget *parent) :
        QDialog(parent), ui(new Ui::SettingDialog) {
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setWindowTitle(tr("Setting"));
    ui->setupUi(this);
    initData();
}

SettingDialog::~SettingDialog() {
    delete ui;
}

void SettingDialog::initData() {
    UiSetting *setting = UiSetting::getInstance();
    on_btnRefresh_clicked();
    ui->edtSavePath->setText(setting->saveMenu);
}

void SettingDialog::on_btnRefresh_clicked() {
    UiSetting *setting = UiSetting::getInstance();
    ui->cbDevices->clear();
    ui->cbDevices->addItems(Adb::devices());
    int index = ui->cbDevices->findText(setting->ipPort);
    if (index > -1) ui->cbDevices->setCurrentIndex(index);
}

void SettingDialog::on_btnSave_clicked() {
    UiSetting *setting = UiSetting::getInstance();
    setting->ipPort = ui->cbDevices->currentText();
    setting->saveMenu = ui->edtSavePath->text();
    setting->saveIni();
}

void SettingDialog::on_btnSavePath_clicked() {
    QString dir = QFileDialog::getExistingDirectory(this);
    if (!dir.isEmpty()) ui->edtSavePath->setText(dir);
}

