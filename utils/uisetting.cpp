#include "uisetting.h"
#include <QCoreApplication>
#include <QSettings>

void UiSetting::saveIni() const {
    QSettings setting("app.ini", QSettings::IniFormat);
    setting.beginGroup("app");
    setting.setValue("ipPort", ipPort);
    setting.setValue("saveMenu", saveMenu);
    setting.setValue("zoomWidth", zoomWidth);
    setting.setValue("zoomHeight", zoomHeight);
    setting.endGroup();
}

UiSetting *UiSetting::getInstance() {
    static UiSetting *instance = nullptr;

    if (instance == nullptr) {
        instance = new UiSetting;
        QSettings setting("app.ini", QSettings::IniFormat);
        instance->ipPort = setting.value("/app/ipPort", "").toString();
        instance->saveMenu = setting.value("/app/saveMenu", "").toString();
        instance->zoomWidth = setting.value("/app/zoomWidth", "0").toInt();
        instance->zoomHeight = setting.value("/app/zoomHeight", "0").toInt();
    }
    return instance;
}
