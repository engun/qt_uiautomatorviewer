#include "adb.h"
#include <QProcess>
#include <QDebug>
#include "uisetting.h"


auto setting = UiSetting::getInstance();

Adb::Adb(QObject *parent)
    : QObject{parent}
{
}

// 同步
bool Adb::exec(const QString& command, QString& output, QString& errOutput)
{
    QStringList commands(command.split(" "));
    QString exe(commands.at(0));
    commands.removeAt(0);
    QProcess process;
    process.start(exe, commands);
    process.waitForFinished(5 * 60 * 1000);
    process.waitForBytesWritten();
    output = QString::fromLocal8Bit(process.readAllStandardOutput()).trimmed();
    errOutput = QString::fromLocal8Bit(process.readAllStandardError()).trimmed();
    return errOutput.isEmpty();
}

// 同步
bool Adb::adb(QString command, QString& output, QString& errOutput)
{

    QString ipPort = device();
    if (ipPort.isEmpty()) {
        errOutput = "no device";
        return false;
    }
    command.replace("%s", "-s " + ipPort);
    qDebug() << command;
    return exec(command, output, errOutput);
}


QString Adb::exec(const QString& adb)
{
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
