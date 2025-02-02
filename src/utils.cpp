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

Path utils::configDirPath() {
  Path path = Path(sago::getConfigHome()) / "guu";
  if (std::filesystem::exists(path))
    return path;
  else if (std::filesystem::create_directories(path))
    return path;
  else
    return Path();
}

Path utils::tempDirPath() {
  Path path = Path(sago::getCacheDir()) / "gaytorrent";
  if (std::filesystem::exists(path))
    return path;
  else if (std::filesystem::create_directories(path))
    return path;
  else
    return Path();
}

Path utils::logPath() { return utils::configDirPath() / "log.txt"; }

byteData
utils::createTorrent(Path path, bool forceV1,
                     std::function<void(int, int)> const &progressCallback) {
  lt::file_storage fs;
  lt::add_files(fs, path.string());

  lt::create_flags_t flags = {};
  if (forceV1)
    flags = lt::create_torrent::v1_only;

  lt::create_torrent t(fs, 0, flags);
  t.add_tracker("http://tracker.gaytor.rent:2710/announce");
  String creator = "GayTor.rent Upload Utility v" + std::to_string(VERSION);
  t.set_creator(creator.c_str());
  t.set_priv(true);

  int pieces = t.num_pieces();
  lt::set_piece_hashes(t, path.parent_path().string(),
                       [progressCallback, pieces](lt::piece_index_t const p) {
                         progressCallback((int)p, pieces);
                       });

  byteData torrent = t.generate_buf();
  return torrent;
}

int utils::fetchLatestVersion() {
  String url = "https://api.github.com/repos/vancer0/guu-cpp/releases/latest";

  auto r = cpr::Get(cpr::Url{url}, cpr::Header{{"User-Agent", "GUU Updater"}},
                    cpr::ConnectTimeout{WEB_TIMEOUT});

  if (r.status_code != 200)
    return 0;

  try {
    json data = json::parse(r.text);
    String ver = data["tag_name"];
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

bool utils::checkIfCommandExists(String cmd) {
#ifndef _WIN32
  String tmp1 = "which " + cmd + " > /dev/null 2>&1";
  String tmp2 = cmd + " --version";
  if (!system(tmp1.c_str()))
    return true;
  else if (!system(tmp2.c_str()))
    return true;
#endif
  return std::filesystem::exists(cmd);
}

Path utils::autoDetectqBitTorrentPath() {
  std::vector<Path> usual = {
#ifdef __linux__
      "/usr/bin/qbittorrent",
      "flatpak run org.qbittorrent.qBittorrent",
#elif _WIN32
      "C:/Program Files/qBittorrent/qbittorrent.exe",
#elif __APPLE__
      "/Applications/qbittorrent.app/Contents/MacOS/qbittorrent",
#endif
  };

  for (Path path : usual)
    if (checkIfCommandExists(path.string()))
      return path;

  return "";
}

#ifdef _WIN32
void utils::installWindowsUpdate(String url) {
  Path tmpPath = tempDirPath() / "guu-update.exe";

  std::ofstream of(tmpPath, std::ios::binary);
  cpr::Response r = cpr::Download(of, cpr::Url{url},
                                  cpr::Header{{"User-Agent", "GUU Updater"}});
  of.close();

  String command = "powershell.exe \"& '" + tmpPath.string() +
                   "'\" /SILENT /FORCECLOSEAPPLICATIONS";
  int returnCode = system(command.c_str());

  if (returnCode != 0) {
    QDesktopServices::openUrl(
        QUrl(QString::fromStdString(url), QUrl::TolerantMode));
  }
}

Path utils::autoDetectUTorrentPath() {
  std::vector<Path> usual = {Path(sago::getDataHome()) /
                                 "uTorrent/uTorrent.exe",
                             "C:/Program Files (x86)/uTorrent/uTorrent.exe"};

  for (Path path : usual)
    if (checkIfCommandExists(path.string()))
      return path;

  return "";
}
#endif

void utils::fetchMessageFromServer()
{
    String url = "https://raw.githubusercontent.com/vancer0/guu-cpp/refs/heads/online/data.json";

    auto r = cpr::Get(cpr::Url{url},
                      cpr::Header{{"User-Agent", "GUU Updater"}},
                      cpr::ConnectTimeout{WEB_TIMEOUT});

    if (r.status_code != 200) {
        throw std::runtime_error("Error connecting to server (" + std::to_string(r.status_code)
                                 + "): " + r.error.message);
    }

    try {
        json data = json::parse(r.text);
        auto msg = data["startupMessage"];
        String title = msg["title"];
        String message = msg["message"];

        if (title != "")
            QMessageBox::information(nullptr,
                                     QString::fromStdString(title),
                                     QString::fromStdString(message));
    } catch (const std::exception &e) {
        throw std::runtime_error(String("Error parsing message: ") + e.what());
    }
}
