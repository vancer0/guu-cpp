#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QMessageBox>
#include <QSplashScreen>
#include <QStyleFactory>
#include <filesystem>

#include "api.h"
#include "constants.h"
#include "settings.h"
#include "utils.h"

void debugOut(QtMsgType type, const QMessageLogContext &context,
              const QString &msg) {
  FILE *log;
  log = fopen(utils::logPath().string().c_str(), "a");

  QByteArray localMsg = msg.toLocal8Bit();
  switch (type) {
  case QtInfoMsg:
    fprintf(log, "INFO | %s | %s\n", context.function, localMsg.constData());
    break;
  case QtWarningMsg:
    fprintf(log, "WARNING | %s | %s\n", context.function, localMsg.constData());
    break;
  case QtCriticalMsg:
    fprintf(log, "CRITICAL | %s | %s\n", context.function,
            localMsg.constData());
    break;
  default:
    break;
  }

  fclose(log);
}

int main(int argc, char *argv[]) {
  std::filesystem::remove(utils::logPath());
  qInstallMessageHandler(debugOut);

  QApplication a(argc, argv);
  a.setApplicationName("GayTor.rent Upload Utility");
  a.setStyle(QStyleFactory::create("Fusion"));
  QPalette p = qApp->palette();
  p.setColor(QPalette::Highlight, QColor(210, 102, 53));
  qApp->setPalette(p);

  qInfo() << a.applicationName() << VERSION;

  qInfo() << "Showing splash screen";
  QPixmap pixmap(":/splash.png");
  QSplashScreen *splash = new QSplashScreen(pixmap);
  splash->show();
  splash->showMessage("Launching...");

  // Idk why this works, but it fixes the splash
  for (int i = 0; i < 100000; i++)
    a.processEvents();

  Settings settings;
  API api;

  qInfo() << "Loading settings";
  splash->showMessage("Loading settings...");
  a.processEvents();
  try {
    settings.load();
    qInfo() << "Loaded settings";
    splash->showMessage("Loaded settings!");
    a.processEvents();
  } catch (const std::exception &e) {
    qWarning() << Q_FUNC_INFO << "Failed to load settings:" << e.what();
    splash->showMessage("Settings reset!");
    a.processEvents();
  }
  settings.save();

  if (settings.updateCheck) {
    qInfo() << "Checking for updates";
    splash->showMessage("Checking for updates...");
    a.processEvents();
    utils::checkForUpdates(false);
  }

  qInfo() << "Fetching startup message";
  splash->showMessage("Fetching startup message...");
  a.processEvents();
  utils::fetchMessageFromServer();

  if (settings.saveLogin) {
    qInfo() << "Performing startup login";
    splash->showMessage("Logging in...");
    a.processEvents();
    api.login(settings.gtUsername, settings.gtPassword);
    if (api.isLoggedIn()) {
      qInfo() << "Logged in";
      splash->showMessage("Logged in!");
      a.processEvents();
    } else {
      qWarning() << "Error logging in:" << api.getLastStatusCode()
                 << api.getLastError().message;
      splash->showMessage("Could not log in.");
      a.processEvents();
    }
  }

  MainWindow w(nullptr, &settings, &api);
  a.processEvents();
  w.show();
  a.processEvents();
  splash->finish(&w);
  return a.exec();
}
