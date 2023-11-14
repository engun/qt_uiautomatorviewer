#pragma once

#include <QDialog>

namespace Ui {
    class OptionDialog;
}

class OptionDialog : public QDialog {
Q_OBJECT

public:
    explicit OptionDialog(QWidget *parent = nullptr);

    ~OptionDialog() override;

private slots:

    void on_btnRefresh_clicked();

    void on_btnSave_clicked();

    void on_btnSavePath_clicked();

private:
    Ui::OptionDialog *ui;

    void initData();
};