#include "api.h"
#include "constants.h"

#undef max
#undef min
#include "jwt-cpp/traits/nlohmann-json/traits.h"

String API::Domains[] = {"https://www.gaytor.rent", "https://www.gaytorrent.ru"};

API::API(int urlIdx)
{
    Url = Domains[urlIdx];
    try {
        categories.loadFromFile();
    } catch (...) {
    }
}

bool API::isServerOnline() {
  auto r = cpr::Head(cpr::Url{Url});

  LastError = r.error;
  LastStatusCode = r.status_code;

  return r.status_code == 200;
}

bool API::login(String username, String password) {
    auto r = gtPost(cpr::Url{Url + "/takelogin.php"},
                    cpr::Multipart{{"username", username}, {"password", password}});

    Cookies = r.cookies;
    LastError = r.error;
    LastStatusCode = r.status_code;

    bool success = this->isLoggedIn();

    if (success) {
        this->downloadCategories();
    }

  return success;
}

bool API::isLoggedIn() {
    auto r = gtGet(cpr::Url{Url + "/qtm.php"});

    LastError = r.error;
    LastStatusCode = r.status_code;

    return r.status_code == 200;
}

bool API::logout() {
    auto r = gtGet(cpr::Url{Url + "/logout.php"});

    Cookies = r.cookies;
    LastError = r.error;
    LastStatusCode = r.status_code;

    return !this->isLoggedIn();
}

bool API::hasCategories() { return !categories.isEmpty(); }

void API::downloadCategories() {
  if (!this->isLoggedIn())
    return;

  auto r = gtGet(cpr::Url{Url + "/genrelist.php"});

  LastError = r.error;
  LastStatusCode = r.status_code;

  try {
    categories.parseFromApi(r.text);
    categories.saveToFile();
  } catch (...) {
  }
}

Categories API::getCategories() { return categories.get(); }

String API::fetchUsername() {
  if (!this->isLoggedIn())
    return "";

  String token = "";
  for (cpr::Cookie cookie : Cookies) {
    if (cookie.GetName() == "token")
      token = cookie.GetValue();
  }

  if (token == "")
    return "";

  auto decoded = jwt::decode(token);

  return decoded.get_payload_json()["username"].get<String>();
}

bool API::clearUploadPictures() {
    auto r = gtPost(cpr::Url{Url + "/doupload.php"}, cpr::Multipart{{"delpic", "Remove"}});

    LastError = r.error;
    LastStatusCode = r.status_code;

    return r.status_code == 200;
}

String API::upload(UploadData data,
                   const std::function<void(int, int)> &callback) {
  this->clearUploadPictures();
  int totalPics = data.picPaths.size();
  int totalSteps = totalPics + 1;

  for (int i = 0; i < totalPics; i++) {
    callback(i, totalSteps);
    Path pic = data.picPaths[i];
    cpr::Multipart picUplData{{"upload", "Upload"}, {"ulpic", cpr::File{pic.string()}}};
    auto r = gtPost(cpr::Url{Url + "/doupload.php"}, picUplData);

    if (r.status_code != 200) {
      throw std::runtime_error("Error uploading picture " + pic.string() +
                               " | " + std::to_string(r.status_code) + " " +
                               r.error.message);
    }
  }
  callback(totalPics, totalSteps);

  cpr::Multipart uplData{};

  uplData.parts.push_back({"MAX_FILE_SIZE", 40000000});
  uplData.parts.push_back({"type", data.mainCateg});
  uplData.parts.push_back({"scat1", data.sCateg1});
  uplData.parts.push_back({"scat2", data.sCateg2});
  uplData.parts.push_back({"scat3", data.sCateg3});
  uplData.parts.push_back({"scat4", data.sCateg4});
  uplData.parts.push_back({"name", data.title});
  uplData.parts.push_back({"qtm", "yes"});
  uplData.parts.push_back({"descr", data.description});
  uplData.parts.push_back({"checktorrent", 1});
  uplData.parts.push_back(
      {"file",
       cpr::Buffer{data.torrent.begin(), data.torrent.end(), "upl.torrent"}});

  auto r = gtPost(cpr::Url{Url + "/doupload.php"}, uplData);

  callback(totalSteps, totalSteps);

  LastError = r.error;
  LastStatusCode = r.status_code;

  if (r.status_code == 200) {
    return r.url.str();
  } else {
    throw std::runtime_error(
        "Error completing upload: " + std::to_string(r.status_code) + " " +
        r.error.message);
  }
}

bool API::download(String url, Path path) {
  String id = "";

  std::regex const e{R"~(\?id=([^&]*))~"};
  auto it = std::sregex_iterator(url.begin(), url.end(), e);
  auto end = std::sregex_iterator();
  for (; it != end; ++it) {
    auto match = *it;
    auto element = match[1].str();
    id = element;
  }

  if (id == "")
    return false;

  String dlUrl = Url + "/download.php/" + id + "/dl.torrent";

  std::ofstream of(path, std::ios::binary);
  cpr::Response r = gtDownload(of, cpr::Url{dlUrl});

  LastError = r.error;
  LastStatusCode = r.status_code;

  return r.status_code == 200;
}
