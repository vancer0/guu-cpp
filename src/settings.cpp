#include "settings.h"
#include "nlohmann/json.hpp"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

using json = nlohmann::json;

Settings::Settings() {
  configPath = utils::configDirPath() + "/guu-config.json";
}

Settings::Settings(str path) { configPath = path; }

void Settings::load() {
  std::ifstream ifs(configPath);
  if (!ifs)
    throw std::runtime_error("Settings: Error opening settings file.");

  std::string content((std::istreambuf_iterator<char>(ifs)),
                      (std::istreambuf_iterator<char>()));

  try {
    auto cfg = json::parse(content);

    updateCheck = cfg.value("updateCheck", updateCheck);
    saveLogin = cfg.value("saveLogin", saveLogin);
    gtUsername = cfg.value("gtUsername", gtUsername);
    gtPassword = cfg.value("gtPassword", gtPassword);
    autoDl = cfg.value("autoDl", autoDl);
    client = cfg.value("client", client);
    qBitHost = cfg.value("qBitHost", qBitHost);
    qBitPort = cfg.value("qBitPort", qBitPort);
    qBitUsername = cfg.value("qBitUsername", qBitUsername);
    qBitPassword = cfg.value("qBitPassword", qBitPassword);
#ifdef _WIN32
    uTorrentPath = cfg.value("uTorrentPath", uTorrentPath);
#endif
    saveUploads = cfg.value("saveUploads", saveUploads);
    savePath = cfg.value("savePath", savePath);
  } catch (json::parse_error &e) {
    std::string s = "Settings: Error loading settings:";
    throw std::runtime_error(s + e.what());
  } catch (...) {
    throw std::runtime_error("Settings: Unknown error while loading settings.");
  }
}

void Settings::save() {
#ifdef _WIN32
  if (uTorrentPath == "")
    uTorrentPath = utils::autoDetectUTorrentPath();
#endif

  json cfg{
      {"updateCheck", updateCheck},
      {"saveLogin", saveLogin},
      {"gtUsername", gtUsername},
      {"gtPassword", gtPassword},
      {"autoDl", autoDl},
      {"client", client},
      {"qBitHost", qBitHost},
      {"qBitPort", qBitPort},
      {"qBitUsername", qBitUsername},
      {"qBitPassword", qBitPassword},
#ifdef _WIN32
      {"uTorrentPath", uTorrentPath},
#endif
      {"saveUploads", saveUploads},
      {"savePath", savePath},
  };

  std::ofstream out(configPath);
  if (!out)
    throw std::runtime_error("Settings: Error creating settings file.");

  out << cfg.dump();
  out.close();

  if (out.bad() || out.fail())
    throw std::runtime_error("Settings: Error writing to settings file.");
}
