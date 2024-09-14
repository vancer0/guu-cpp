cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)

project(guu-cpp VERSION 15 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(platform_folders REQUIRED)
find_package(LibtorrentRasterbar REQUIRED)

set(CPR_USE_SYSTEM_CURL ON)
set(BUILD_SHARED_LIBS OFF)
include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG 3fbe8028471663acb2ab5a68c7e75b6fc9b85557)
FetchContent_MakeAvailable(cpr)

find_path(JWT_CPP_INCLUDE_DIRS "jwt-cpp/base.h")

add_subdirectory(src)

