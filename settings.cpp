#include "settings.h"
#include "utils.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

Settings::Settings() {
    configPath = utils::configDirPath() + "/guu-config.json";
}

Settings::Settings(str path) {
    configPath = path;
}

void Settings::load() {
    std::ifstream ifs(configPath);
    std::string content( (std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));

    auto cfg = json::parse(content);

    cfg.at("language").get_to(language);
    cfg.at("saveLogin").get_to(saveLogin);
    cfg.at("gtUsername").get_to(gtUsername);
    cfg.at("gtPassword").get_to(gtPassword);
    cfg.at("autoDl").get_to(autoDl);
    cfg.at("client").get_to(client);
    cfg.at("webUiHost").get_to(qBitHost);
    cfg.at("webUiPort").get_to(qBitPort);
    cfg.at("webUiUsername").get_to(qBitUsername);
    cfg.at("webUiPassword").get_to(qBitPassword);
    cfg.at("saveUploads").get_to(saveUploads);
    cfg.at("savePath").get_to(savePath);
}

void Settings::save() {
    json cfg{
        {"language", language},
        {"saveLogin", saveLogin},
        {"gtUsername", gtUsername},
        {"gtPassword", gtPassword},
        {"autoDl", autoDl},
        {"client", client},
        {"webUiHost", qBitHost},
        {"webUiPort", qBitPort},
        {"webUiUsername", qBitUsername},
        {"webUiPassword", qBitPassword},
        {"saveUploads", saveUploads},
        {"savePath", savePath},
    };

    std::ofstream out(configPath);
    out << cfg.dump();
    out.close();
}
