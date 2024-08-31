#ifndef API_H
#define API_H

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <map>
#include <cpr/cpr.h>
#include "categories.h"

#define TIMEOUT 5000 // ms
#define UPL_TIMEOUT 120000 // ms

typedef std::string str;

class API
{
private:
    str Url;
    cpr::Cookies Cookies;
    Categories categories;

    cpr::Error LastError;
public:
    struct UploadData {
        std::vector<char> torrent;
        std::vector<str> picPaths;
        str mainCateg;
        str sCateg1;
        str sCateg2;
        str sCateg3;
        str sCateg4;
        str title;
        str description;
    };

    API();
    API(str url) : Url(url) {}

    void updateURL(str url) { Url = url; }
    bool isServerOnline();
    cpr::Error getLastError() { return LastError; }

    bool login(str username, str password);
    bool isLoggedIn();
    bool logout();

    bool hasCategories();
    void downloadCategories();
    std::map<str, str> getCategories();
    str fetchUsername();

    bool clearUploadPictures();
    str upload(UploadData data);
    bool download(str url, str path);
};

#endif // API_H
