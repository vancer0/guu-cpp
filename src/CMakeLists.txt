configure_file(Version.h.in Version.h @ONLY)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

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
)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

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
