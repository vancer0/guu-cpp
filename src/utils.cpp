#include "utils.h"
#include "constants.h"
#include "cpr/cpr.h"
#include "libtorrent/create_torrent.hpp"
#include "nlohmann/json.hpp"

#include <QDebug>
#include <QDesktopServices>
#include <QMessageBox>
#include <QUrl>

using json = nlohmann::json;

str utils::configDirPath() {
  str path = sago::getConfigHome() + "/guu";
  if (std::filesystem::exists(path))
    return path;
  else if (std::filesystem::create_directories(path))
    return path;
  else
    return "";
}

str utils::tempDirPath() {
  str path = sago::getCacheDir() + "/gaytorrent";
  if (std::filesystem::exists(path))
    return path;
  else if (std::filesystem::create_directories(path))
    return path;
  else
    return "";
}

str utils::logPath() { return utils::configDirPath() + "/log.txt"; }

std::vector<char> utils::createTorrent(str path, str parentDir) {
  lt::file_storage fs;
  lt::add_files(fs, path);
  lt::create_torrent t(fs);
  t.add_tracker("http://tracker.gaytor.rent:2710/announce", 0);
  str creator = "GayTor.rent Upload Utility v" + std::to_string(VERSION);
  t.set_creator(creator.c_str());
  lt::set_piece_hashes(t, parentDir, [](lt::piece_index_t const p) {});

  std::vector<char> torrent = t.generate_buf();
  return torrent;
}

int utils::fetchLatestVersion() {
  str url = "https://api.github.com/repos/vancer0/guu-cpp/releases/latest";

  auto r = cpr::Get(cpr::Url{url}, cpr::Header{{"User-Agent", "GUU Updater"}},
                    cpr::Timeout{3000});

  if (r.status_code != 200)
    return 0;

  try {
    json data = json::parse(r.text);
    str ver = data["tag_name"];
    return std::stoi(ver);
  } catch (...) {
    return 0;
  }
}

void utils::checkForUpdates(bool msgIfLatest) {
  int latestVer = utils::fetchLatestVersion();
  if (latestVer > VERSION) {
    qInfo() << "Found new version:" << latestVer;
    QMessageBox msgBox;
    msgBox.setWindowTitle("GUU - Updater");
    msgBox.setText("A new version of GUU is available (v" +
                   QString::number(latestVer) +
                   "). Would you like to download it?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    msgBox.setIcon(QMessageBox::Question);

    if (msgBox.exec() == QMessageBox::Yes) {
      QString url = "https://github.com/vancer0/guu-cpp/releases/download/";
      url += QString::number(latestVer);
#ifdef _WIN32
      url += "/GUU-Windows-Installer-x86_64.exe";
      utils::installWindowsUpdate(url.toStdString());
#else
#ifdef __linux__
      url += "/GUU-Linux-x86_64.AppImage";
#elif __APPLE__
      url += "/GUU-Mac-x86_64.dmg";
#endif // __linux__, __APPLE__
      QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
#endif // WIN32
      return;
    }
  } else if (msgIfLatest) {
    QMessageBox::information(nullptr, "GUU - Updater", "No new updates found!");
  }
  qInfo() << "No new version found";
}

bool utils::checkIfCommandExists(str path) {
#ifndef _WIN32
  str tmp1 = "which " + path + " > /dev/null 2>&1";
  str tmp2 = path + " --version";
  if (!system(tmp1.c_str()))
    return true;
  else if (!system(tmp2.c_str()))
    return true;
#endif
  return std::filesystem::exists(path);
}

str utils::autoDetectqBitTorrentPath() {
  std::vector<str> usual = {
#ifdef __linux__
      "/usr/bin/qbittorrent",
      "flatpak run org.qbittorrent.qBittorrent",
#elif _WIN32
      "C:\\Program Files\\qBittorrent\\qbittorrent.exe",
#elif __APPLE__
      "/Applications/qbittorrent.app/Contents/MacOS/qbittorrent",
#endif
  };

  for (auto path : usual)
    if (checkIfCommandExists(path))
      return path;

  return "";
}

#ifdef _WIN32
void utils::installWindowsUpdate(str url) {
  str tmpPath = tempDirPath() + "/guu-update.exe";

  std::ofstream of(tmpPath, std::ios::binary);
  cpr::Response r = cpr::Download(of, cpr::Url{url},
                                  cpr::Header{{"User-Agent", "GUU Updater"}});
  of.close();

  str command =
      "powershell.exe \"& '" + tmpPath + "'\" /SILENT /FORCECLOSEAPPLICATIONS";
  int returnCode = system(command.c_str());

  if (returnCode != 0) {
    QDesktopServices::openUrl(
        QUrl(QString::fromStdString(url), QUrl::TolerantMode));
  }
}

str utils::autoDetectUTorrentPath() {
  std::vector<str> usual = {sago::getDataHome() + "\\uTorrent\\uTorrent.exe",
                            "C:\\Program Files (x86)\\uTorrent\\uTorrent.exe"};

  for (auto path : usual)
    if (checkIfCommandExists(path))
      return path;

  return "";
}
#endif
