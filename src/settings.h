#ifndef SETTINGS_H
#define SETTINGS_H

#include <map>
#include <string>
#include <vector>

typedef std::string str;

class Settings {
public:
  Settings();
  Settings(str path);

  void save();
  void load();

  const std::vector<str> Clients{
      "qBitTorrent",
#ifdef _WIN32
      "uTorrent",
#endif
  };

  bool updateCheck = true;

  bool saveLogin = false;
  str gtUsername = "";
  str gtPassword = "";

  bool autoDl = false;
  str client = Clients[0];
  str qBitHost = "localhost";
  str qBitPort = "8080";
  str qBitUsername = "admin";
  str qBitPassword = "";

#ifdef _WIN32
  str uTorrentPath = "";
#endif

  bool saveUploads = false;
  str savePath = "";

private:
  str configPath;
};

#endif // SETTINGS_H
