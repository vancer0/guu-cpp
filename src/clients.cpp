#include "clients.h"
#include "utils.h"
#include <QClipboard>
#include <QDesktopServices>
#include <QGuiApplication>
#include <QMessageBox>
#include <QProcess>
#include <QUrl>
#include <filesystem>
#include <iostream>
#include <stdexcept>

#ifdef _WIN32
#include <cstdlib>
#include <iterator>
#endif

// System Handler

bool SystemTorrentHandler::addTorrent(str torrent, str localPath) {
  QClipboard *clipboard = QGuiApplication::clipboard();
  clipboard->setText(QString::fromStdString(localPath));
  QUrl url = QUrl("file:///" + QString::fromStdString(torrent));
  QDesktopServices::openUrl(url);
  return true;
}

// qBitTorrent WebUI

void qBitTorrentWeb::configure(Settings *settings) {
  if (settings == nullptr) {
    throw std::runtime_error("Client: Invalid settings object");
  }

  webUiUrl = settings->qBitHost;

  if (!settings->qBitPort.empty() || settings->qBitPort != "80")
    webUiUrl += ":" + settings->qBitPort;

  if (webUiUrl.rfind("http", 0) != 0)
    webUiUrl = "http://" + webUiUrl;

  auto r = cpr::Post(cpr::Url{webUiUrl + "/api/v2/auth/login"},
                     cpr::Multipart{{"username", settings->qBitUsername},
                                    {"password", settings->qBitPassword}},
                     cpr::Timeout{CLIENT_TIMEOUT});
  if (r.status_code == 0)
    throw std::runtime_error("Client: Could not connect to qBitTorrent");

  if (r.text == "Ok.")
    header = cpr::Header{{"Cookie", "SID=" + r.cookies[0].GetValue()}};
}

bool qBitTorrentWeb::isConnected() {
  auto r = cpr::Get(cpr::Url{webUiUrl + "/api/v2/app/version"}, header,
                    cpr::Timeout{CLIENT_TIMEOUT});

  return r.status_code == 200;
}

bool qBitTorrentWeb::addTorrent(str torrent, str localPath) {
  auto r = cpr::Post(cpr::Url{webUiUrl + "/api/v2/torrents/add"}, header,
                     cpr::Timeout{CLIENT_UPL_TIMEOUT},
                     cpr::Multipart{
                         {"torrents", cpr::File{torrent}},
                         {"savepath", localPath},
                         {"paused", "false"},
                     });

  return r.status_code == 200;
}

qBitTorrentWeb::~qBitTorrentWeb() {
  cpr::Post(cpr::Url{webUiUrl + "/api/v2/auth/logout"}, header,
            cpr::Timeout{CLIENT_TIMEOUT});
}

// qBitTorrent Local

void qBitTorrent::configure(Settings *settings) {
  if (settings == nullptr) {
    throw std::runtime_error("Client: Invalid settings object");
  }

  if (std::filesystem::exists(settings->qBitPath))
    Path = settings->qBitPath;
  else
    throw std::runtime_error("Client: Could not find specified file");
}

bool qBitTorrent::isConnected() { return utils::checkIfCommandExists(Path); }

bool qBitTorrent::addTorrent(str torrent, str localPath) {
  QString command = QString::fromStdString(Path);
  QStringList arguments({QString::fromStdString(torrent), "--skip-dialog=true",
                         "--save-path=" + QString::fromStdString(localPath)});

  QProcess::startDetached(command, arguments);
  return true;
}

// uTorrent

#ifdef _WIN32
void uTorrent::configure(Settings *settings) {
  if (settings == nullptr) {
    throw std::runtime_error("Client: Invalid settings object");
  }

  if (std::filesystem::exists(settings->uTorrentPath))
    Path = settings->uTorrentPath;
  else
    throw std::runtime_error("Client: Could not find specified file");
}

bool uTorrent::isConnected() { return std::filesystem::exists(Path); }

bool uTorrent::addTorrent(str torrent, str localPath) {
  QString command = QString::fromStdString(Path);
  QStringList arguments({"/minimized", "/directory",
                         QString::fromStdString(localPath),
                         QString::fromStdString(torrent)});

  QProcess::startDetached(command, arguments);
  return true;
}
#endif
