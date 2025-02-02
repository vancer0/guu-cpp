#include "settings.h"
#include "nlohmann/json.hpp"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

using json = nlohmann::json;

Settings::Settings() {
  configPath = utils::configDirPath() / "guu-config.json";
}

Settings::Settings(Path path) { configPath = path; }

void Settings::load() {
  std::ifstream ifs(configPath);
  if (!ifs)
    throw std::runtime_error("Settings: Error opening settings file.");

  String content((std::istreambuf_iterator<char>(ifs)),
                 (std::istreambuf_iterator<char>()));

  try {
    auto cfg = json::parse(content);

    updateCheck = cfg.value("updateCheck", updateCheck);
    saveWarn = cfg.value("saveWarn", saveWarn);
    saveLogin = cfg.value("saveLogin", saveLogin);
    forceV1 = cfg.value("forceV1", forceV1);
    gtDomain = cfg.value("gtDomain", gtDomain);
    gtUsername = cfg.value("gtUsername", gtUsername);
    gtPassword = cfg.value("gtPassword", gtPassword);
    autoDl = cfg.value("autoDl", autoDl);
    client = cfg.value("client", client);
    qBitPath = cfg.value("qBitPath", qBitPath);
    qBitHost = cfg.value("qBitHost", qBitHost);
    qBitPort = cfg.value("qBitPort", qBitPort);
    qBitUsername = cfg.value("qBitUsername", qBitUsername);
    qBitPassword = cfg.value("qBitPassword", qBitPassword);
#ifdef _WIN32
    uTorrentPath = cfg.value("uTorrentPath", uTorrentPath);
#endif
    saveUploads = cfg.value("saveUploads", saveUploads);
    savePath = cfg.value("savePath", savePath);
  } catch (const std::exception &e) {
    String s = "Settings: Error loading settings:";
    throw std::runtime_error(s + e.what());
  }
}

void Settings::save() {
#ifdef _WIN32
  if (uTorrentPath.empty())
    uTorrentPath = utils::autoDetectUTorrentPath().string();
#endif
  if (qBitPath.empty())
    qBitPath = utils::autoDetectqBitTorrentPath().string();

  if (gtUsername.empty() || gtPassword.empty())
    saveLogin = false;
  if (savePath.empty())
    saveUploads = false;

  json cfg{
      {"updateCheck", updateCheck},
      {"saveWarn", saveWarn},
      {"saveLogin", saveLogin},
      {"forceV1", forceV1},
      {"gtDomain", gtDomain},
      {"gtUsername", gtUsername},
      {"gtPassword", gtPassword},
      {"autoDl", autoDl},
      {"client", client},
      {"qBitPath", qBitPath},
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

  out << cfg.dump(2);
  out.close();

  if (out.bad() || out.fail())
    throw std::runtime_error("Settings: Error writing to settings file.");
}
