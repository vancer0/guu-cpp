#ifndef API_H
#define API_H

#include "categories.h"
#include "cpr/cpr.h"
#include "types.h"
#include <functional>
#include <iostream>
#include <map>
#include <optional>
#include <regex>
#include <vector>

class API {
private:
  String Url;
  cpr::Cookies Cookies;
  CategoriesParser categories;
  static String Domains[];

  cpr::Error LastError;
  int LastStatusCode = -1;

public:
  struct UploadData {
    byteData torrent;
    std::vector<Path> picPaths;
    String mainCateg;
    String sCateg1;
    String sCateg2;
    String sCateg3;
    String sCateg4;
    String title;
    String description;
  };

  API(int urlIdx);

  void updateURL(int urlIdx) { Url = Domains[urlIdx]; }
  bool isServerOnline();
  cpr::Error getLastError() { return LastError; }
  int getLastStatusCode() { return LastStatusCode; }

  bool login(String username, String password);
  bool isLoggedIn();
  bool logout();

  bool hasCategories();
  void downloadCategories();
  Categories getCategories();
  String fetchUsername();

  bool clearUploadPictures();
  String upload(UploadData data, const std::function<void(int, int)> &callback);
  bool download(String url, Path path);
};

#endif // API_H
