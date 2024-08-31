cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)

project(guu-cpp VERSION 10 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(cpr REQUIRED)
find_package(jwt-cpp REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(platform_folders REQUIRED)
find_package(LibtorrentRasterbar REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(guu-cpp
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        api.h api.cpp
        aboutwindow.h aboutwindow.cpp aboutwindow.ui
        constants.h
        loginwindow.h loginwindow.cpp loginwindow.ui
        settings.h settings.cpp
        utils.h
        settingswindow.h settingswindow.cpp settingswindow.ui
        categories.h categories.cpp
        utils.cpp
        clients.h clients.cpp
        picturelist.cpp picturelist.h
        uploadworker.h uploadworker.cpp
        assets/icon.qrc
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET guu-cpp APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(guu-cpp SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(guu-cpp
            ${PROJECT_SOURCES}
        )
    endif()
endif()

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

target_link_libraries(guu-cpp PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(guu-cpp PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(guu-cpp PRIVATE jwt-cpp::jwt-cpp)
target_link_libraries(guu-cpp PRIVATE cpr::cpr)
target_link_libraries(guu-cpp PRIVATE sago::platform_folders)
target_link_libraries(guu-cpp PRIVATE LibtorrentRasterbar::torrent-rasterbar)


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.guu-cpp)
endif()
set_target_properties(guu-cpp PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS guu-cpp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(guu-cpp)
endif()
