#ifndef UISETTING_H
#define UISETTING_H

#include <QObject>

class UiSetting {
    explicit UiSetting();

public:
    QString ipPort;
    QString saveMenu;
    int zoomWidth{};
    int zoomHeight{};

    void saveIni() const;

    static UiSetting *getInstance();

private:
    static UiSetting *loadIni();
};

#endif // UISETTING_H
