#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <vector>
#include <map>

typedef std::string str;

class Settings
{
public:
    Settings();
    Settings(str path);

    void save();
    void load();

    const std::map<std::string, std::string> Languages {
        {"en_US", "English (US)"},
    };
    const std::vector<str> Clients {"qBitTorrent"};

    str language = Languages.begin()->first;

    bool saveLogin = false;
    str gtUsername = "";
    str gtPassword = "";

    bool autoDl = true;
    str client = Clients[0];
    str qBitHost = "localhost";
    str qBitPort = "8080";
    str qBitUsername = "admin";
    str qBitPassword = "";

    bool saveUploads = false;
    str savePath = "";

private:
    str configPath;
};

#endif // SETTINGS_H
