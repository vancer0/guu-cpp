#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include "clients.h"
#include "settings.h"
#include <QPushButton>
#include <QWidget>
#include <algorithm>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget {
  Q_OBJECT

public:
  explicit SettingsWindow(QWidget *parent = nullptr);
  ~SettingsWindow();
  void setData(TorrentClient *client = nullptr, Settings *cfg = nullptr);

  QPushButton *getButton();

  void enableLoginBox();
  void enableClientBox();
  void enableDownloadSetting();
  void updateBoxes();
  void updateClientSettings();
  void hideClientSettings();
  void refresh() {
    this->enableClientBox();
    this->enableLoginBox();
    this->enableDownloadSetting();
  }

  void selectSavePath();
  void selectuTorPath();
  void selectqBitPath();

  void loadSettings();
  void saveSettings();

private:
  Ui::SettingsWindow *ui;
  TorrentClient *Client;
  Settings *Cfg;

signals:
  void saved();
};

#endif // SETTINGSWINDOW_H
