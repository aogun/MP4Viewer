cmake_minimum_required(VERSION 3.10)
file(READ "src/version.h" ver)
string(REGEX MATCH "MP4_VIEWER_VERSION_MAJOR ([0-9]*)" _ ${ver})
set(ver_major ${CMAKE_MATCH_1})

string(REGEX MATCH "MP4_VIEWER_VERSION_MINOR ([0-9]*)" _ ${ver})
set(ver_minor ${CMAKE_MATCH_1})

string(REGEX MATCH "MP4_VIEWER_VERSION_PATCH ([0-9]*)" _ ${ver})
set(ver_patch ${CMAKE_MATCH_1})

message("version: ${ver_major}.${ver_minor}.${ver_patch}")
project(MP4Viewer VERSION ${ver_major}.${ver_minor}.${ver_patch})

set(CMAKE_CXX_STANDARD 17)

set(BUILD_APPS OFF)

add_subdirectory(deps)
add_subdirectory(src)

# CPack
set(CPACK_PACKAGE_VENDOR "MP4Viewer")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "mp4/mov file viewer/editor.")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/aogun/MP4Viewer")
set(CPACK_PACKAGE_CONTACT "angus <angus_ag@msn.com>")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md") # TODO: Where is this used? Do we need a better
# source?

if(WIN32)
    # Include all dynamically linked runtime libraries such as MSVCRxxx.dll
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "./")
    include(InstallRequiredSystemLibraries)
    set(CPACK_PACKAGE_FILE_NAME ${PROJECT_NAME}-${PROJECT_VERSION})
    set(CPACK_GENERATOR ZIP)
endif()

include(CPack)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/deps/ffmpeg/bin/ DESTINATION "/")