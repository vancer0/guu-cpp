#include "mainwindow.h"

#include <QApplication>
#include <QDesktopServices>
#include <QMessageBox>
#include <QSplashScreen>

#include "api.h"
#include "constants.h"
#include "settings.h"
#include "utils.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  a.setApplicationName("Gaytor.rent Upload Utility");

  QPixmap pixmap(":/splash.png");
  QSplashScreen *splash = new QSplashScreen(pixmap, Qt::WindowStaysOnTopHint);
  splash->show();
  splash->showMessage("Launching...");

  /// Idk why this works, but it fixes the splash
  for (int i = 0; i < 100000; i++)
    a.processEvents();

  Settings settings;
  API api;

  splash->showMessage("Loading settings...");
  a.processEvents();
  try {
    settings.load();
    splash->showMessage("Loaded settings!");
    a.processEvents();
  } catch (const std::exception &e) {
    QMessageBox::warning(nullptr, "GUU - Error",
                         QString::fromStdString(e.what()));
    splash->showMessage("Settings reset!");
    a.processEvents();
  }
  settings.save();

  if (settings.updateCheck) {
    splash->showMessage("Checking for updates...");
    a.processEvents();
    utils::checkForUpdates();
  }

  if (settings.saveLogin) {
    splash->showMessage("Logging in...");
    a.processEvents();
    api.login(settings.gtUsername, settings.gtPassword);
    splash->showMessage("Logged in!");
    a.processEvents();
  }

  MainWindow w(nullptr, &settings, &api);
  a.processEvents();
  w.show();
  a.processEvents();
  splash->finish(&w);
  return a.exec();
}
