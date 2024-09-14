#include "clients.h"
#include "constants.h"
#include "utils.h"
#include <QClipboard>
#include <QDesktopServices>
#include <QGuiApplication>
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

bool SystemTorrentHandler::addTorrent(Path torrent, Path localPath) {
  QClipboard *clipboard = QGuiApplication::clipboard();
  clipboard->setText(QString::fromStdString(localPath.string()));
  QUrl url = QUrl(QString::fromStdString("file:///" + torrent.string()));
  QDesktopServices::openUrl(url);
  return true;
}

// qBitTorrent WebUI

void qBitTorrentWeb::configure(Settings *settings) {
  if (settings == nullptr) {
    throw std::runtime_error("Client: Invalid settings object");
  }

  _webUiUrl = settings->qBitHost;

  if (!settings->qBitPort.empty() || settings->qBitPort != "80")
    _webUiUrl += ":" + settings->qBitPort;

  if (_webUiUrl.rfind("http", 0) != 0)
    _webUiUrl = "http://" + _webUiUrl;

  auto r = cpr::Post(cpr::Url{_webUiUrl + "/api/v2/auth/login"},
                     cpr::Multipart{{"username", settings->qBitUsername},
                                    {"password", settings->qBitPassword}},
                     cpr::ConnectTimeout{WEB_TIMEOUT});
  if (r.status_code == 0)
    throw std::runtime_error("Client: Could not connect to qBitTorrent");

  if (r.text == "Ok.")
    _header = cpr::Header{{"Cookie", "SID=" + r.cookies[0].GetValue()}};
}

bool qBitTorrentWeb::isConnected() {
  auto r = cpr::Get(cpr::Url{_webUiUrl + "/api/v2/app/version"}, _header,
                    cpr::ConnectTimeout{WEB_TIMEOUT});

  return r.status_code == 200;
}

bool qBitTorrentWeb::addTorrent(Path torrent, Path localPath) {
  auto r = cpr::Post(cpr::Url{_webUiUrl + "/api/v2/torrents/add"}, _header,
                     cpr::ConnectTimeout{WEB_TIMEOUT},
                     cpr::Multipart{
                         {"torrents", cpr::File{torrent.string()}},
                         {"savepath", localPath.string()},
                         {"paused", "false"},
                     });

  return r.status_code == 200;
}

qBitTorrentWeb::~qBitTorrentWeb() {
  cpr::Post(cpr::Url{_webUiUrl + "/api/v2/auth/logout"}, _header,
            cpr::ConnectTimeout{WEB_TIMEOUT});
}

// qBitTorrent Local

void qBitTorrent::configure(Settings *settings) {
  if (settings == nullptr) {
    throw std::runtime_error("Client: Invalid settings object");
  }

  if (std::filesystem::exists(settings->qBitPath))
    _path = settings->qBitPath;
  else
    throw std::runtime_error("Client: Could not find specified file");
}

bool qBitTorrent::isConnected() {
  return utils::checkIfCommandExists(_path.string());
}

bool qBitTorrent::addTorrent(Path torrent, Path localPath) {
  QString command = QString::fromStdString(_path.string());
  QStringList arguments(
      {QString::fromStdString(torrent.string()), "--skip-dialog=true",
       "--save-path=" + QString::fromStdString(localPath.string())});

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
    _path = settings->uTorrentPath;
  else
    throw std::runtime_error("Client: Could not find specified file");
}

bool uTorrent::isConnected() { return std::filesystem::exists(_path); }

bool uTorrent::addTorrent(Path torrent, Path localPath) {
  QString command = QString::fromStdString(_path.string());
  QStringList arguments({"/minimized", "/directory",
                         QString::fromStdString(localPath.string()),
                         QString::fromStdString(torrent.string())});

  QProcess::startDetached(command, arguments);
  return true;
}
#endif
