#pragma once

#include <QMainWindow>
#include <QResizeEvent>
#include <QPixmap>
#include <QLabel>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QPainter>
#include "utils/managetreexml.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private slots:

    void on_actOpen_triggered();

    void on_treWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_btnExpand_clicked();

    void on_edtSearch_textChanged(const QString &arg1);

    void on_btnLast_clicked();

    void on_btnNext_clicked();

    void on_actScreenshot_triggered();

    void on_actSetting_triggered();

private:
    void initView();

    void initTableNodeIfNeed(const QList<QString> &keys);

    void resizeEvent(QResizeEvent *event) override;

    bool eventFilter(QObject *watched, QEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    QPoint picturePos2ActualPos(const QPoint &pos);

    QPoint actualPos2picturePos(const QPoint &pos);

    QRect nodeBounds(QTreeWidgetItem *item);

    QRect nodeBounds(const QMap<QString, QString> &map);

    void showPicture(const QString &pngFile, const QString &uixFile);

    void showNodeDetail(QTreeWidgetItem *treeWidgetItem);

private:
    Ui::MainWindow *ui;
    QPixmap *originalPixmap{};
    QPixmap actualPixmap;
    ManageTreeXml xml;
    QString pngPath;
    QString uixPath;
    Qt::PenStyle penStyle{Qt::DotLine};
    QTreeWidgetItem *currentItem{};
    QRect solidLineRect;
    QList<QTreeWidgetItem *> searchResultItems;
    int searchResultIndex{};
};
