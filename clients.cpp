#include "clients.h"
#include <iostream>

void qBitTorrent::configure(Settings &settings) {
    webUiUrl = settings.qBitHost + ":" + settings.qBitPort;

    if (webUiUrl.rfind("http", 0) != 0)
        webUiUrl = "http://" + webUiUrl;

    auto r = cpr::Post(cpr::Url{webUiUrl + "/api/v2/auth/login"},
                       cpr::Multipart{{"username", settings.qBitUsername},
                                      {"password", settings.qBitPassword}},
                       cpr::Timeout{CLIENT_TIMEOUT});
    if (r.text == "Ok.")
        header = cpr::Header{{"Cookie", "SID=" + r.cookies[0].GetValue()}};
}

bool qBitTorrent::isConnected() {
    auto r = cpr::Get(cpr::Url{webUiUrl + "/api/v2/app/version"},
                      header,
                      cpr::Timeout{CLIENT_TIMEOUT});

    return r.status_code == 200;
}

bool qBitTorrent::addTorrent(std::vector<char> torrent, str localPath) {
    auto r = cpr::Post(cpr::Url{webUiUrl + "/api/v2/torrents/add"},
                       header,
                       cpr::Timeout{CLIENT_TIMEOUT},
                       cpr::Multipart{
                           {"torrents", cpr::Buffer{torrent.begin(),
                                                    torrent.end(),
                                                    "upl.torrent"}},
                           {"savepath", localPath},
                           {"paused", "false"},
                       });

    return r.status_code == 200;
}

qBitTorrent::~qBitTorrent() {
    cpr::Post(cpr::Url{webUiUrl + "/api/v2/auth/logout"},
              header,
              cpr::Timeout{CLIENT_TIMEOUT});
}
