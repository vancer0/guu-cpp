#ifndef UTILS_H
#define UTILS_H

#include "sago/platform_folders.h"
#include <filesystem>
#include <string>
#include <vector>

namespace utils {
std::string configDirPath();
std::string tempDirPath();
void openUri(std::string uri);
std::vector<char> createTorrent(std::string path, std::string parentDir);
int fetchLatestVersion();
void checkForUpdates();

#ifdef _WIN32
std::string autoDetectUTorrentPath();
#endif
} // namespace utils

#endif // UTILS_H
