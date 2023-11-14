#include "uisetting.h"
#include <QCoreApplication>
#include <QSettings>

UiSetting *instance = nullptr;

UiSetting::UiSetting() = default;

void UiSetting::saveIni() const {
    QSettings setting("app.ini", QSettings::IniFormat);
    setting.beginGroup("app");
    setting.setValue("ipPort", ipPort);
    setting.setValue("saveMenu", saveMenu);
    setting.setValue("zoomWidth", zoomWidth);
    setting.setValue("zoomHeight", zoomHeight);
    setting.endGroup();
}

UiSetting *UiSetting::loadIni() {
    QSettings setting("app.ini", QSettings::IniFormat);
    auto *uiSetting = new UiSetting();
    uiSetting->ipPort = setting.value("/app/ipPort", "").toString();
    uiSetting->saveMenu = setting.value("/app/saveMenu", "").toString();
    uiSetting->zoomWidth = setting.value("/app/zoomWidth", "0").toInt();
    uiSetting->zoomHeight = setting.value("/app/zoomHeight", "0").toInt();
    return uiSetting;
}

UiSetting *UiSetting::getInstance() {
    if (instance == nullptr) {
        instance = loadIni();
    }
    return instance;
}
