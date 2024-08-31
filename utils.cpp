#include "utils.h"
#include "constants.h"
#include "libtorrent/create_torrent.hpp"
#include <fstream>

std::string utils::configDirPath() {
    std::string path = sago::getConfigHome() + "/guu";
    if (std::filesystem::exists(path))
        return path;
    else if (std::filesystem::create_directories(path))
        return path;
    else
        return "";
}

std::string utils::tempDirPath() {
    std::string path = sago::getCacheDir() + "/gaytorrent";
    if (std::filesystem::exists(path))
        return path;
    else if (std::filesystem::create_directories(path))
        return path;
    else
        return "";
}

std::vector<char> utils::createTorrent(std::string path, std::string parentDir) {
    lt::file_storage fs;
    lt::add_files(fs, path);
    lt::create_torrent t(fs);
    t.add_tracker("http://tracker.gaytor.rent:2710/announce", 0);
    std::string creator = "GayTor.rent Upload Utility v" + std::to_string(VERSION);
    t.set_creator(creator.c_str());
    lt::set_piece_hashes(t, parentDir, [] (lt::piece_index_t const p) {});

    std::vector<char> torrent = t.generate_buf();
    return torrent;
}
