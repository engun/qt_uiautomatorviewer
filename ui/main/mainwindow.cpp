#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils/uisetting.h"
#include "utils/adb.h"
#include "ui/option/settingdialog.h"
#include <QFileDialog>

#include <QDebug>
#include <QPainter>
#include <QMessageBox>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    initView();
}

MainWindow::~MainWindow() {
    delete ui;
    delete originalPixmap;
}

void MainWindow::initView() {
    ui->lblPicture->move(ui->widget->pos());
    ui->lblPicture->resize(ui->widget->size());
    ui->lblPicture->installEventFilter(this);
}


void MainWindow::on_actOpen_triggered() {
    UiSetting *setting = UiSetting::getInstance();
    QString pngFile = QFileDialog::getOpenFileName(this, QString(), setting->saveMenu, "*.png");
    if (pngFile.isEmpty()) return;
    QString uixFile = QString(pngFile).toLower().replace(".png", ".xml");
    showPicture(pngFile, uixFile);
    xml.loadTreeXml(ui->treWidget, uixFile);
}

void MainWindow::showPicture(const QString &pngFile, const QString &uixFile) {
    // init
    penStyle = Qt::DotLine;
    delete originalPixmap;
    searchResultItems.clear();

    this->pngPath = pngFile;
    this->uixPath = uixFile;
    if (pngFile.isEmpty() || uixFile.isEmpty()) return;
    originalPixmap = new QPixmap(pngFile);
    xml.loadTreeXml(ui->treWidget, uixFile);
    if (originalPixmap != nullptr) {
        ui->lblPicture->resize(ui->widget->size());
        actualPixmap = originalPixmap->scaled(ui->lblPicture->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        repaint();
    }
}

void MainWindow::resizeEvent(QResizeEvent *event) {

    if (event->size() != event->oldSize()) {
        penStyle = Qt::DotLine;
        showPicture(pngPath, uixPath);
    }
    QMainWindow::resizeEvent(event);
}

QPoint MainWindow::picturePos2ActualPos(const QPoint &pos) {
    QSize actualSize = actualPixmap.size();
    int x0 = (ui->lblPicture->width() - actualSize.width()) / 2;
    int y0 = (ui->lblPicture->height() - actualSize.height()) / 2;
    int x = pos.x() - x0;
    int y = pos.y() - y0;

    x = x * originalPixmap->width() / (actualPixmap.width() - 1);
    y = y * originalPixmap->height() / (actualPixmap.height() - 1);
    return {x, y};
}

QPoint MainWindow::actualPos2picturePos(const QPoint &pos) {
    QSize actualSize = actualPixmap.size();
    int x = pos.x() * actualPixmap.width() / (originalPixmap->width() - 1);
    int y = pos.y() * actualPixmap.height() / (originalPixmap->height() - 1);

    int x0 = (ui->lblPicture->width() - actualSize.width()) / 2;
    int y0 = (ui->lblPicture->height() - actualSize.height()) / 2;

    return {x + x0, y + y0};
}


void MainWindow::on_treWidget_itemClicked(QTreeWidgetItem *item, int column) {
    qDebug() << "colum: " << column;
    showNodeDetail(item);
    // display box
    auto map = item->data(0, Qt::UserRole).value<QMap<QString, QString>>();
    solidLineRect = nodeBounds(map);
    penStyle = Qt::NoPen;
    repaint();
}

void MainWindow::showNodeDetail(QTreeWidgetItem *treeWidgetItem) {
    auto map = treeWidgetItem->data(0, Qt::UserRole).value<QMap<QString, QString>>();
    initTableNodeIfNeed(map.keys());
    // display node detail
    for (int row = 0; row < map.size(); ++row) {
        auto item = ui->tblNode->item(row, 0);
        ui->tblNode->item(row, 1)->setText(map[item->text()]);
    }
}

void MainWindow::initTableNodeIfNeed(const QList<QString> &keys) {
    if (keys.size() == ui->tblNode->rowCount()) return;
    ui->tblNode->setRowCount((int) keys.size());
    ui->tblNode->setColumnCount(2);
    for (int row = 0; row < keys.size(); ++row) {
        for (int col = 0; col < 2; ++col) {
            QString value = col == 0 ? keys.at(row) : "";
            auto *item = new QTableWidgetItem();
            item->setText(value);
            ui->tblNode->setItem(row, col, item);
        }
    }
}

QRect MainWindow::nodeBounds(QTreeWidgetItem *item) {
    auto map = item->data(0, Qt::UserRole).value<QMap<QString, QString>>();
    // 显示方框
    return nodeBounds(map);
}

QRect MainWindow::nodeBounds(const QMap<QString, QString> &map) {
    // 显示方框
    QString bound = map["bounds"];
    bound = bound.replace("][", ",");
    bound.replace("[", "");
    bound.replace("]", "");
    auto parts = bound.split(",");
    QPoint topLeft(parts[0].toInt(), parts[1].toInt());
    QPoint bottomRight(parts[2].toInt(), parts[3].toInt());
    QRect rect(topLeft, bottomRight);
    return {actualPos2picturePos(rect.topLeft()), actualPos2picturePos(rect.bottomRight())};
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->lblPicture) {
        if (event->type() == QEvent::Paint) {
            if (originalPixmap != nullptr) {
                QPainter painter(ui->lblPicture);
                painter.drawPixmap(actualPos2picturePos(QPoint(0, 0)), actualPixmap);
                if (penStyle == Qt::DotLine || penStyle == Qt::SolidLine) {
                    auto picturePos = ui->lblPicture->mapFromGlobal(QCursor::pos());
                    auto actualPos = picturePos2ActualPos(picturePos);
                    currentItem = ManageTreeXml::containsPosSmallest(ui->treWidget, actualPos);
                    if (currentItem != nullptr) {
                        QRect rect = nodeBounds(currentItem);
                        ui->treWidget->setCurrentItem(currentItem);
                        showNodeDetail(currentItem);
                        painter.setPen(QPen(Qt::red, 2, penStyle));
                        painter.drawRect(rect);
                        if (penStyle == Qt::SolidLine) {
                            penStyle = Qt::NoPen;
                            solidLineRect = rect;
                        }
                    }
                } else if (penStyle == Qt::NoPen) {
                    painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));
                    painter.drawRect(solidLineRect);
                }
            }
        } else if (event->type() == QEvent::MouseMove && originalPixmap != nullptr) {
            auto picturePos = ui->lblPicture->mapFromGlobal(QCursor::pos());
            auto actualPos = picturePos2ActualPos(picturePos);
            if (penStyle != Qt::NoPen
                && actualPos.x() >= 0
                && actualPos.y() >= 0
                && actualPos.x() <= originalPixmap->width()
                && actualPos.y() <= originalPixmap->height()) {
                ui->lblPos->setText(QString("%1, %2").arg(actualPos.x()).arg(actualPos.y()));
                repaint();
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (penStyle == Qt::DotLine) {
            penStyle = Qt::SolidLine;
        } else if (penStyle == Qt::NoPen) {
            penStyle = Qt::DotLine;
        }
        repaint();
    }
}

void MainWindow::on_btnExpand_clicked() {
    ui->treWidget->expandAll();
}

void MainWindow::on_edtSearch_textChanged(const QString &arg1) {
    searchResultItems = xml.search(ui->treWidget, arg1);
    if (!searchResultItems.isEmpty()) {
        auto item = searchResultItems.at(0);
        ui->treWidget->setCurrentItem(item);
        emit ui->treWidget->itemClicked(item, 0);
        searchResultIndex = 0;
    }
}

void MainWindow::on_btnLast_clicked() {
    if (!searchResultItems.isEmpty()) {
        searchResultIndex--;
        if (searchResultIndex < 0) searchResultIndex = (int) searchResultItems.size() - 1;
        auto item = searchResultItems.at(searchResultIndex);
        ui->treWidget->setCurrentItem(item);
        emit ui->treWidget->itemClicked(item, 0);
    }
}

void MainWindow::on_btnNext_clicked() {
    if (!searchResultItems.isEmpty()) {
        searchResultIndex++;
        if (searchResultIndex >= searchResultItems.size()) searchResultIndex = 0;
        auto item = searchResultItems.at(searchResultIndex);
        ui->treWidget->setCurrentItem(item);
        emit ui->treWidget->itemClicked(item, 0);
    }
}

void MainWindow::on_actScreenshot_triggered() {
    QString uiDumpPath = QDir::tempPath() + "/uidump.xml";
    QString uiShotPath = QDir::tempPath() + "/uidump.png";

    auto setting = UiSetting::getInstance();

    if (!setting->saveMenu.isEmpty()) {
        QDir dir(setting->saveMenu);

        if (dir.exists()) {
            QString dateString = QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
            uiDumpPath = setting->saveMenu + "/" + dateString + ".xml";
            uiShotPath = setting->saveMenu + "/" + dateString + ".png";
        }
    }

    static QString UIAUTOMATOR = "/system/bin/uiautomator";
    static QString UIAUTOMATOR_DUMP_COMMAND = "dump";
    static QString UIDUMP_DEVICE_PATH = "/data/local/tmp/uidump.xml";
    static QString UIDUMP_SHOT_PATH = "/data/local/tmp/uishot.png";
    static QString ADB_UIDUMP_DEVICE_DIR = "/data/local/tmp/local/tmp/";
    static QString ADB_UIDUMP_DEVICE_PATH = ADB_UIDUMP_DEVICE_DIR + "uidump.xml";

    static QString UIAUTOMATOR_TEST_COMMAND = "runtest LvmamaXmlKit.jar";
    static QString UIAUTOMATOR_TEST_COMMAND_ARGS = "-c com.lvmama.uidump.DumpXml";

    QString output, errOutput;


    if (!Adb::devices().contains(setting->ipPort)) setting->ipPort = "";
    QString currentPath = QCoreApplication::applicationDirPath();

    // 检查路径是否含有空格
    QStringList paths = {
            uiDumpPath, uiShotPath, currentPath
    };
    for (auto &path: paths) {
        if (path.contains(" ")) {
            QMessageBox::critical(this, "错误", "路径不能有空格: " + path);
            return;
        }
    }

    QStringList commands = QStringList()
            << "adb devices"
            << "adb %s shell mkdir -p " + ADB_UIDUMP_DEVICE_DIR
            << QString("adb %s push %1 /data/local/tmp/").arg(currentPath + "/LvmamaXmlKit.jar")
            << QString("adb %s shell %1 %2 %3").arg(UIAUTOMATOR, UIAUTOMATOR_TEST_COMMAND,
                                                    UIAUTOMATOR_TEST_COMMAND_ARGS)
            << "adb %s shell mv " + ADB_UIDUMP_DEVICE_PATH + " /data/local/tmp"
            << QString("adb %s pull %1 %2").arg(UIDUMP_DEVICE_PATH, uiDumpPath)
            << QString("adb %s pull %1 %2").arg(UIDUMP_SHOT_PATH, uiShotPath)
            << "adb %s shell rm " + UIDUMP_SHOT_PATH
            << "adb %s shell rm " + UIDUMP_DEVICE_PATH;

    bool ret = false;
    int size = (int) commands.size();
    for (int i = 0; i < size; ++i) {
        const QString &command = commands.at(i);
        ret = Adb::adb(command, output, errOutput);
        if (ret || (i != 3 && i != 5 && i != 6)) {
            ret = true;
            ui->progressBar->setValue((i + 1) * (100 / size));
        } else {
            break;
        }
    }
    ui->progressBar->setValue(100);
    if (ret) {
        pngPath = uiShotPath;
        uixPath = uiDumpPath;
        showPicture(pngPath, uixPath);
    } else {
        QMessageBox::critical(this, "错误", errOutput);
    }
}

// 设置
void MainWindow::on_actSetting_triggered() {
    SettingDialog option(this);
    option.exec();
}

