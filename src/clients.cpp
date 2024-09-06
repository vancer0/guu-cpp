#include "clients.h"
#include <filesystem>
#include <iostream>
#include <stdexcept>

#ifdef _WIN32
#include "utils.h"
#include <cstdlib>
#include <iterator>
#endif

void qBitTorrent::configure(Settings *settings) {
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

bool qBitTorrent::isConnected() {
  auto r = cpr::Get(cpr::Url{webUiUrl + "/api/v2/app/version"}, header,
                    cpr::Timeout{CLIENT_TIMEOUT});

  return r.status_code == 200;
}

bool qBitTorrent::addTorrent(std::vector<char> torrent, str localPath) {
  auto r =
      cpr::Post(cpr::Url{webUiUrl + "/api/v2/torrents/add"}, header,
                cpr::Timeout{CLIENT_UPL_TIMEOUT},
                cpr::Multipart{
                    {"torrents", cpr::Buffer{torrent.begin(), torrent.end(),
                                             "upl.torrent"}},
                    {"savepath", localPath},
                    {"paused", "false"},
                });

  return r.status_code == 200;
}

qBitTorrent::~qBitTorrent() {
  cpr::Post(cpr::Url{webUiUrl + "/api/v2/auth/logout"}, header,
            cpr::Timeout{CLIENT_TIMEOUT});
}

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

bool uTorrent::addTorrent(std::vector<char> torrent, str localPath) {
  str tmpPath = utils::tempDirPath() + "/temp.torrent";
  std::filesystem::remove(tmpPath);

  std::ofstream file(tmpPath, std::ios::out | std::ios::binary);
  if (!file)
    return false;

  copy(torrent.cbegin(), torrent.cend(), std::ostreambuf_iterator<char>(file));

  str command = "powershell.exe \"& '" + Path + "'\" /DIRECTORY \"" +
                localPath + "\" \"" + tmpPath + "\"";

  int returnCode = system(command.c_str());

  return returnCode == 0;
}
#endif
