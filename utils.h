#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include "sago/platform_folders.h"
#include <filesystem>

namespace utils {
std::string configDirPath();
std::string tempDirPath();
void openUri(std::string uri);
std::vector<char> createTorrent(std::string path, std::string parentDir);
}

#endif // UTILS_H
