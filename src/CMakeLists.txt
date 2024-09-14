configure_file(Version.h.in Version.h @ONLY)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

set(MACOSX_BUNDLE_ICON_FILE icon.icns)
set(app_icon_macos assets/icon.icns)
set_source_files_properties(${app_icon_macos} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")

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
    assets/splash.qrc
    assets/guu.ico assets/icon.rc
    ${app_icon_macos}
    picturemanager.h picturemanager.cpp picturemanager.ui
    types.h
)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
if(WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

target_include_directories(guu-cpp PRIVATE ${JWT_CPP_INCLUDE_DIRS})

target_link_libraries(guu-cpp PRIVATE Qt6::Widgets)
target_link_libraries(guu-cpp PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(guu-cpp PRIVATE cpr::cpr)
target_link_libraries(guu-cpp PRIVATE sago::platform_folders)
target_link_libraries(guu-cpp PRIVATE LibtorrentRasterbar::torrent-rasterbar)

set_target_properties(guu-cpp PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER io.github.vancer0.guu
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS guu-cpp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(guu-cpp)
