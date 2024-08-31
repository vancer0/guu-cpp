#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QPushButton>
#include <QWidget>
#include <algorithm>
#include "settings.h"
#include "clients.h"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

    QPushButton *getButton();

    void enableLoginBox();
    void enableClientBox();
    void enableDownloadSetting();
    void updateBoxes(Settings &settings);
    void refresh() {
        this->enableClientBox();
        this->enableLoginBox();
        this->enableDownloadSetting();
    }

    void selectSavePath();

    void applySettings(Settings &settings);
    void saveSettings(Settings &settings);

private:
    Ui::SettingsWindow *ui;
};

#endif // SETTINGSWINDOW_H
