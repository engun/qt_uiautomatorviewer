#pragma once

#include <QString>

class UiSetting {

public:
    QString ipPort;
    QString saveMenu;
    int zoomWidth{};
    int zoomHeight{};

    void saveIni() const;

    static UiSetting *getInstance();
};