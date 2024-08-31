#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <string>

#include "picturelist.h"
#include "aboutwindow.h"
#include "loginwindow.h"
#include "settingswindow.h"
#include "api.h"
#include "settings.h"
#include "clients.h"
#include "uploadworker.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void openAboutWindow() {
        AboutWin.show();
    }
    void openLoginWindow() {
        LoginWin.show();
    }
    void openSettingsWindow() {
        SettingsWin.applySettings(settings);
        SettingsWin.show();
    }
    void applySettings() {
        SettingsWin.saveSettings(settings);
        this->loadTorrentClient();
        this->refreshInfo();
        SettingsWin.hide();
    }

    void reloadCategories();
    void loadCategories();
    void enableItemsAll(bool enable);
    void enableItemsAuto();

    void loadTorrentClient();

    void login();
    void logout();
    void updateStatus();
    void refreshInfo() {
        this->loadCategories();
        this->updateStatus();
    }

    void selectFile();
    void selectFolder();
    void selectPictures();
    void removePictures();

    void clearAllFields();
    void openProject();
    void saveProject();

    void uploadChecks();
    void beginUpload();
    void showUploadError(const QString &text);
    void finishUpload();

    void openWebUI();

private:
    Ui::MainWindow *ui;

    Settings settings;

    AboutWindow AboutWin;
    LoginWindow LoginWin;
    SettingsWindow SettingsWin;

    TorrentClient *Client = nullptr;

    QThread *thread;
    UploadWorker *worker;

    API Api;
};
#endif // MAINWINDOW_H
