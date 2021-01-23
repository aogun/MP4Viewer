cmake_minimum_required(VERSION 3.10)
project(MP4Viewer VERSION 0.1.2)

set(CMAKE_CXX_STANDARD 17)


add_subdirectory(deps)
add_subdirectory(src)

# CPack
set(CPACK_PACKAGE_VENDOR "personal")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "mp4/mov file viewer/editor.")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/aogun/MP4Viewer")
set(CPACK_PACKAGE_CONTACT "angus <angus_ag@msn.com>")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md") # TODO: Where is this used? Do we need a better
# source?

if(WIN32)
    # Include all dynamically linked runtime libraries such as MSVCRxxx.dll
    include(InstallRequiredSystemLibraries)
    set(CPACK_PACKAGE_FILE_NAME ${PROJECT_NAME}-${PROJECT_VERSION})
    set(CPACK_GENERATOR ZIP)
endif()

include(CPack)