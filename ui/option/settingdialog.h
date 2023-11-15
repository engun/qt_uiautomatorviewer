#pragma once

#include <QDialog>

namespace Ui {
    class SettingDialog;
}

class SettingDialog : public QDialog {
Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = nullptr);

    ~SettingDialog() override;

private slots:

    void on_btnRefresh_clicked();

    void on_btnSave_clicked();

    void on_btnSavePath_clicked();

private:
    Ui::SettingDialog *ui;

    void initData();
};