#ifndef UTILS_H
#define UTILS_H

#include "sago/platform_folders.h"
#include <filesystem>
#include <string>
#include <vector>

typedef std::string str;

namespace utils {
str configDirPath();
str tempDirPath();
void openUri(str uri);
std::vector<char> createTorrent(str path, str parentDir);
int fetchLatestVersion();
void checkForUpdates(bool msgIfLatest);

#ifdef _WIN32
void installWindowsUpdate(str url);
str autoDetectUTorrentPath();
#endif
} // namespace utils

#endif // UTILS_H
