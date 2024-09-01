#include "api.h"
#include <sstream>
#include "jwt-cpp/jwt.h"
#include "jwt-cpp/traits/kazuho-picojson/defaults.h"

API::API() {
    Url = "https://gaytor.rent";
    try {
        categories.loadFromFile();
    } catch (...) {}
}

bool API::isServerOnline() {
    auto r = cpr::Head(cpr::Url{Url},
                       cpr::Timeout{TIMEOUT});

    LastError = r.error;

    return r.status_code == 200;
}

bool API::login(str username, str password) {
    auto r = cpr::Post(cpr::Url{Url + "/takelogin.php"},
                                cpr::Multipart{{"username", username},
                                               {"password", password}},
                                cpr::Timeout{TIMEOUT});

    Cookies = r.cookies;
    LastError = r.error;

    bool success = this->isLoggedIn();

    if (success) {
        this->downloadCategories();
    }

    return success;
}

bool API::isLoggedIn() {
    bool success1 = false;
    for (cpr::Cookie cookie : Cookies) {
        if (cookie.GetName() == "token") {
            success1 = true;
            break;
        }
    }

    auto r = cpr::Get(cpr::Url{Url + "/qtm.php"},
                      Cookies,
                      cpr::Timeout{TIMEOUT});
    bool success2 = r.status_code == 200;

    LastError = r.error;

    return success1 && success2;
}

bool API::logout() {
    auto r = cpr::Get(cpr::Url{Url + "/logout.php"},
                      Cookies,
                      cpr::Timeout{TIMEOUT});

    Cookies = r.cookies;
    LastError = r.error;

    return !this->isLoggedIn();
}

bool API::hasCategories() {
    return !categories.isEmpty();
}

void API::downloadCategories() {
    if (!this->isLoggedIn())
        return;

    auto r = cpr::Get(cpr::Url{Url + "/genrelist.php"},
                      Cookies,
                      cpr::Timeout{TIMEOUT});

    LastError = r.error;

    try {
        categories.parseFromApi(r.text);
        categories.saveToFile();
    } catch (...) {}
}

std::map<str, str> API::getCategories() {
    return categories.get();
}

str API::fetchUsername() {
    if (!this->isLoggedIn())
        return "";

    str token = "";
    for (cpr::Cookie cookie : Cookies) {
        if (cookie.GetName() == "token")
            token = cookie.GetValue();
    }

    if (token == "")
        return "";

    auto decoded = jwt::decode(token);

    return decoded.get_payload_json()["username"].get<std::string>();
}

bool API::clearUploadPictures() {
    auto r = cpr::Post(cpr::Url{Url + "/doupload.php"},
                       Cookies,
                       cpr::Multipart{{"delpic", "Remove"}});

    LastError = r.error;

    return r.status_code == 200;
}

str API::upload(UploadData data) {
    cpr::Multipart uplData {};

    uplData.parts.push_back({"MAX_FILE_SIZE", 40000000});
    uplData.parts.push_back({"type", data.mainCateg});
    uplData.parts.push_back({"scat1", data.sCateg1});
    uplData.parts.push_back({"scat2", data.sCateg2});
    uplData.parts.push_back({"scat3", data.sCateg3});
    uplData.parts.push_back({"scat4", data.sCateg4});
    uplData.parts.push_back({"name", data.title});
    uplData.parts.push_back({"infourl", ""});
    uplData.parts.push_back({"descr", data.description});
    uplData.parts.push_back({"checktorrent", "Do it!"});
    uplData.parts.push_back({"file", cpr::Buffer{data.torrent.begin(),
                                                 data.torrent.end(),
                                                 "upl.torrent"}});
    for (str pic : data.picPaths)
        uplData.parts.push_back({"ulpic[]", cpr::File{pic}});

    auto r = cpr::Post(cpr::Url{Url + "/doupload.php"},
                       Cookies,
                       uplData);

    LastError = r.error;

    return r.url.str();
}

bool API::download(str url, str path) {
    str id = "";

    std::regex const e{R"~(\?id=([^&]*))~"};
    auto it = std::sregex_iterator(url.begin(), url.end(), e);
    auto end = std::sregex_iterator();
    for(; it != end; ++it) {
        auto match = *it;
        auto element = match[1].str();
        id = element;
    }

    if (id == "")
        return false;

    str dlUrl = Url + "/download.php/" + id + "/dl.torrent";

    std::ofstream of(path, std::ios::binary);
    cpr::Response r = cpr::Download(of,
                                    cpr::Url{dlUrl},
                                    Cookies,
                                    cpr::Timeout{UPL_TIMEOUT});

    LastError = r.error;

    return r.status_code == 200;
}
