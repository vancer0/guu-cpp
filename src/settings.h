#ifndef SETTINGS_H
#define SETTINGS_H

#include "types.h"

class Settings {
public:
  Settings();
  Settings(Path path);

  void save();
  void load();

  const std::vector<String> Clients{
      "System Default",
      "qBitTorrent",
      "qBitTorrent WebUI",
#ifdef _WIN32
      "uTorrent",
#endif
  };

  bool updateCheck = true;
  bool saveWarn = true;

  bool saveLogin = false;
  String gtUsername = "";
  String gtPassword = "";

  bool autoDl = false;
  String client = Clients[0];

  String qBitPath = "";

  String qBitHost = "http://localhost";
  String qBitPort = "8080";
  String qBitUsername = "admin";
  String qBitPassword = "";

#ifdef _WIN32
  String uTorrentPath = "";
#endif

  bool saveUploads = false;
  String savePath = "";

private:
  Path configPath;
};

#endif // SETTINGS_H
