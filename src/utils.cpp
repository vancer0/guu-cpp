#include "utils.h"
#include "constants.h"
#include "cpr/cpr.h"
#include "libtorrent/create_torrent.hpp"
#include "nlohmann/json.hpp"

#include <QDesktopServices>
#include <QMessageBox>
#include <QUrl>

using json = nlohmann::json;

std::string utils::configDirPath() {
  std::string path = sago::getConfigHome() + "/guu";
  if (std::filesystem::exists(path))
    return path;
  else if (std::filesystem::create_directories(path))
    return path;
  else
    return "";
}

std::string utils::tempDirPath() {
  std::string path = sago::getCacheDir() + "/gaytorrent";
  if (std::filesystem::exists(path))
    return path;
  else if (std::filesystem::create_directories(path))
    return path;
  else
    return "";
}

std::vector<char> utils::createTorrent(std::string path,
                                       std::string parentDir) {
  lt::file_storage fs;
  lt::add_files(fs, path);
  lt::create_torrent t(fs);
  t.add_tracker("http://tracker.gaytor.rent:2710/announce", 0);
  std::string creator =
      "GayTor.rent Upload Utility v" + std::to_string(VERSION);
  t.set_creator(creator.c_str());
  lt::set_piece_hashes(t, parentDir, [](lt::piece_index_t const p) {});

  std::vector<char> torrent = t.generate_buf();
  return torrent;
}

int utils::fetchLatestVersion() {
  std::string url =
      "https://api.github.com/repos/vancer0/guu-cpp/releases/latest";

  auto r = cpr::Get(cpr::Url{url}, cpr::Header{{"User-Agent", "GUU"}}, cpr::Timeout{3000});

  if (r.status_code != 200)
    return 0;

  try {
    json data = json::parse(r.text);
    std::string ver = data["tag_name"];
    return std::stoi(ver);
  } catch (...) {
    return 0;
  }
}

void utils::checkForUpdates() {
  int latestVer = utils::fetchLatestVersion();
  if (latestVer > VERSION) {
    QMessageBox msgBox;
    msgBox.setText(
        "A new version of GUU is available. Would you like to download it?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    msgBox.setIcon(QMessageBox::Question);

    if (msgBox.exec() == QMessageBox::Yes) {
      QString url = "https://github.com/vancer0/guu-cpp/releases/download/";
      url += QString::number(latestVer);
#ifdef _WIN32
      url += "/GUU-Windows-Installer-x86_64.exe";
#elif __linux__
      url += "/GUU-Linux-x86_64.AppImage";
#elif __APPLE__
      url += "/GUU-Mac-x86_64.dmg";
#endif

      QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
    }
  }
}

#ifdef _WIN32
std::string utils::autoDetectUTorrentPath()
{
    std::string usual1 = "C:\\Program Files (x86)\\uTorrent\\uTorrent.exe";
    std::string usual2 = sago::getDataHome() + "\\uTorrent\\uTorrent.exe";

    if (std::filesystem::exists(usual1))
        return usual1;
    else if (std::filesystem::exists(usual2))
        return usual2;
    else
        return "";
}
#endif
