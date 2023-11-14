#ifndef ADB_H
#define ADB_H

#include <QObject>

class Adb : public QObject
{
    Q_OBJECT
    void setDeviceIndex(int index);
public:
    explicit Adb(QObject *parent = nullptr);

    static bool exec(const QString& adb, QString &output, QString &errOutput);
    static QString exec(const QString& adb);
    static QStringList devices();
    static QString device();
    static bool adb(QString command, QString &output, QString &errOutput);
signals:

};

#endif // ADB_H
