#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <string>

#include "aboutwindow.h"
#include "api.h"
#include "clients.h"
#include "loginwindow.h"
#include "picturelist.h"
#include "picturemanager.h"
#include "settings.h"
#include "settingswindow.h"
#include "uploadworker.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent, Settings *s, API *a);
  ~MainWindow();
  void uiSetup();

  void openAboutWindow() { AboutWin.show(); }
  void openLoginWindow() { LoginWin.show(); }
  void openPicMgr() { PicMgr.show(); }
  void openSettingsWindow() {
    SettingsWin.updateClientSettings();
    SettingsWin.loadSettings();
    SettingsWin.show();
  }

  void reloadCategories();
  void loadCategories();
  void enableItemsAll(bool enable);
  void enableItemsAuto();

  void loadTorrentClient();

  void logout();
  void updateStatus();
  void refreshInfo() {
    this->loadCategories();
    this->updateStatus();
  }
  bool checkTitle();

  void updatePictures();

  void selectFile();
  void selectFolder();
  void selectPictures();
  void removePictures();

  bool clearAllFields();
  void openProject();
  void saveProject();
  void saveProjectAs();

  void uploadChecks();
  void beginUpload();
  void showUploadError(const QString &text);
  void finishUpload();

  void openWebUI();

private:
  Ui::MainWindow *ui;

  Settings *Cfg = nullptr;

  AboutWindow AboutWin;
  LoginWindow LoginWin;
  SettingsWindow SettingsWin;
  PictureManager PicMgr;

  TorrentClient *Client = nullptr;

  QThread *thread;
  UploadWorker *worker;

  API *Api = nullptr;

  QString lastProjectPath = "Untitled.guu";
  bool _modified = false;

  void closeEvent(QCloseEvent *bar);
};
#endif // MAINWINDOW_H
