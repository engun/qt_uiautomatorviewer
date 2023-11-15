#pragma once

#include <QObject>

class Adb : public QObject {
Q_OBJECT

public:
    explicit Adb(QObject *parent = nullptr);

    static bool exec(const QString &adb, QString &output, QString &errOutput);

    static QString exec(const QString &adb);

    static QStringList devices();

    static QString device();

    static bool adb(QString command, QString &output, QString &errOutput);
};
