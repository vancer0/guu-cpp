#ifndef API_H
#define API_H

#include "categories.h"
#include "cpr/cpr.h"
#include <iostream>
#include <map>
#include <optional>
#include <regex>
#include <string>
#include <vector>

typedef std::string str;

class API {
private:
  str Url;
  cpr::Cookies Cookies;
  Categories categories;

  cpr::Error LastError;
  int LastStatusCode = -1;

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
  int getLastStatusCode() { return LastStatusCode; }

  bool login(str username, str password);
  bool isLoggedIn();
  bool logout();

  bool hasCategories();
  void downloadCategories();
  std::map<str, str> getCategories();
  str fetchUsername();

  bool clearUploadPictures();
  std::optional<str> upload(UploadData data);
  bool download(str url, str path);
};

#endif // API_H
