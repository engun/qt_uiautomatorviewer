#include "optiondialog.h"
#include "ui_optiondialog.h"
#include "utils/uisetting.h"
#include "utils/adb.h"
#include <QFileDialog>

OptionDialog::OptionDialog(QWidget *parent) :
        QDialog(parent), ui(new Ui::OptionDialog) {
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setWindowTitle("设置");
    ui->setupUi(this);
    initData();
}

OptionDialog::~OptionDialog() {
    delete ui;
}

void OptionDialog::initData() {
    UiSetting *setting = UiSetting::getInstance();
    on_btnRefresh_clicked();
    ui->edtSavePath->setText(setting->saveMenu);
}

void OptionDialog::on_btnRefresh_clicked() {
    UiSetting *setting = UiSetting::getInstance();
    ui->cbDevices->clear();
    ui->cbDevices->addItems(Adb::devices());
    int index = ui->cbDevices->findText(setting->ipPort);
    if (index > -1) ui->cbDevices->setCurrentIndex(index);
}


// 保存
void OptionDialog::on_btnSave_clicked() {
    UiSetting *setting = UiSetting::getInstance();
    setting->ipPort = ui->cbDevices->currentText();
    setting->saveMenu = ui->edtSavePath->text();
    setting->saveIni();
}


// 选择保存文件夹
void OptionDialog::on_btnSavePath_clicked() {
    QString dir = QFileDialog::getExistingDirectory(this);
    if (!dir.isEmpty()) ui->edtSavePath->setText(dir);
}

