cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)

project(guu-cpp VERSION 14 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(cpr REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(platform_folders REQUIRED)
find_package(LibtorrentRasterbar REQUIRED)

find_path(JWT_CPP_INCLUDE_DIRS "jwt-cpp/base.h")

add_subdirectory(src)

