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

const String USERAGENT = "Mozilla/5.0 (Windows NT 6.2; WOW64) AppleWebKit/537.36 (QTM-KHTML, like "
                         "Gecko) Chrome/30.0.1599.17 Safari/537.36";

class API {
private:
  String Url;
  cpr::Cookies Cookies;
  CategoriesParser categories;
  static String Domains[];

  cpr::Error LastError;
  int LastStatusCode = -1;

  template<class... Ts>
  cpr::Response gtGet(Ts &&...ts)
  {
      return cpr::Get(std::forward<Ts>(ts)..., Cookies, cpr::Header{{"User-Agent", USERAGENT}});
  }

  template<class... Ts>
  cpr::Response gtPost(Ts &&...ts)
  {
      return cpr::Post(std::forward<Ts>(ts)..., Cookies, cpr::Header{{"User-Agent", USERAGENT}});
  }

  template<class... Ts>
  cpr::Response gtDownload(Ts &&...ts)
  {
      return cpr::Download(std::forward<Ts>(ts)..., Cookies, cpr::Header{{"User-Agent", USERAGENT}});
  }

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
