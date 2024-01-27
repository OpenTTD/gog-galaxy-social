cmake_minimum_required(VERSION 3.16)

project(gog-galaxy-social
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15)

include(RPath)
include(CompileFlags)

if(NOT GOG_GALAXY_CLIENT_ID OR NOT GOG_GALAXY_CLIENT_SECRET)
    message(FATAL_ERROR "Please use -DGOG_GALAXY_CLIENT_ID=<id> -DGOG_GALAXY_CLIENT_SECRET=<secret> to tell the plugin what GOG Galaxy Client ID/secret to use.")
endif()

add_compile_definitions(GOG_GALAXY_CLIENT_ID=\"${GOG_GALAXY_CLIENT_ID}\")
add_compile_definitions(GOG_GALAXY_CLIENT_SECRET=\"${GOG_GALAXY_CLIENT_SECRET}\")

add_library(${PROJECT_NAME} SHARED
    src/plugin.cpp
)

install(TARGETS ${PROJECT_NAME} DESTINATION .)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib)

if(WIN32)
    if ("$ENV{Platform}" STREQUAL "arm64")
        message(FATAL_ERROR "Windows ARM64 is (currently) not supported by the GOG Galaxy SDK")
    else()
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            target_link_libraries(${PROJECT_NAME} PRIVATE Galaxy64)
            set(LIBRARY_DEPENDENCY ${CMAKE_SOURCE_DIR}/lib/Galaxy64.dll)
            install(FILES ${LIBRARY_DEPENDENCY} DESTINATION .)
            target_sources(${PROJECT_NAME} PRIVATE os/windows/library_win64.manifest)
        else()
            target_link_libraries(${PROJECT_NAME} PRIVATE Galaxy)
            set(LIBRARY_DEPENDENCY ${CMAKE_SOURCE_DIR}/lib/Galaxy.dll)
            install(FILES ${LIBRARY_DEPENDENCY} DESTINATION .)
            target_sources(${PROJECT_NAME} PRIVATE os/windows/library_win32.manifest)
        endif()
    endif()
elseif(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE Galaxy64)
    set(LIBRARY_DEPENDENCY ${CMAKE_SOURCE_DIR}/lib/libGalaxy64.dylib)
    install(FILES ${LIBRARY_DEPENDENCY} DESTINATION .)
else()
    message(FATAL_ERROR "Linux is (currently) not supported by the GOG Galaxy SDK")
endif()

include(Version)
include(InstallAndPackage)
