#include "adb.h"
#include <QProcess>
#include <QDebug>
#include "uisetting.h"


auto setting = UiSetting::getInstance();

Adb::Adb(QObject *parent) : QObject{parent} {
}

// 同步
bool Adb::exec(const QString &command, QString &output, QString &errOutput) {
    QProcess process;
    process.startCommand(command);
    process.waitForFinished(5 * 60 * 1000);
    process.waitForBytesWritten();
    output = QString::fromLocal8Bit(process.readAllStandardOutput()).trimmed();
    errOutput = QString::fromLocal8Bit(process.readAllStandardError()).trimmed();
    qDebug().noquote() << "output: " << output;
    if (process.exitCode() != 0) {
        if (errOutput.isEmpty()) errOutput = output;
    }
    return errOutput.isEmpty() && process.exitCode() == 0;
}

// 同步
bool Adb::adb(QString command, QString &output, QString &errOutput) {

    QString ipPort = device();
    if (ipPort.isEmpty()) {
        errOutput = "no device";
        return false;
    }
    command.replace("%s", "-s " + ipPort);
    qDebug().noquote() << command;
    return exec(command, output, errOutput);
}


QString Adb::exec(const QString &adb) {
    QString output, errOutput;
    exec(adb, output, errOutput);
    return output.isEmpty() ? errOutput : output;
}

QStringList Adb::devices() {
    QString part = exec("adb devices");
    auto parts = part.split("\r\n");
    QStringList ipPorts;
    for (int i = 1; i < parts.size(); ++i) {
        ipPorts << parts.at(i).split('\t').at(0);
    }
    return ipPorts;
}


QString Adb::device() {
    if (setting->ipPort.isEmpty()) {
        QStringList deviceList = devices();
        if (deviceList.isEmpty()) return {};
        setting->ipPort = deviceList.at(0);
        setting->saveIni();
        return setting->ipPort;
    }
    return setting->ipPort;
}
