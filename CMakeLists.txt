cmake_minimum_required(VERSION 3.12)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE "Release")

if(APPLE)
    set(CMAKE_APPLE_SILICON_PROCESSOR "arm64")

    message(STATUS "${CMAKE_APPLE_SILICON_PROCESSOR}")

    if(CMAKE_APPLE_SILICON_PROCESSOR STREQUAL "arm64")
        set(CMAKE_OSX_ARCHITECTURES "arm64")
        file(COPY ${CMAKE_SOURCE_DIR}/lib/discord_game_sdk/aarch64/discord_game_sdk.dylib
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/src/ui/)
    elseif(CMAKE_APPLE_SILICON_PROCESSOR STREQUAL "x86_64")
        set(CMAKE_OSX_ARCHITECTURES "x86_64")
        file(COPY ${CMAKE_SOURCE_DIR}/lib/discord_game_sdk/x86_64/discord_game_sdk.dylib
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/src/ui/)
    endif()
elseif (WIN32)
    file(COPY ${CMAKE_SOURCE_DIR}/lib/discord_game_sdk/x86_64/discord_game_sdk.dll
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/src/ui/)
elseif (UNIX AND NOT APPLE) # Linux
    file(COPY ${CMAKE_SOURCE_DIR}/lib/discord_game_sdk/x86_64/discord_game_sdk.so
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/src/ui/)

    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/build/release)
    endif()
endif()

project(Betacraft)

if (NOT DEFINED ENV{BETACRAFT_API_MICROSOFT_CLIENT_ID})
    set(API_MICROSOFT_CLIENT_ID "")
else()
    set(API_MICROSOFT_CLIENT_ID $ENV{BETACRAFT_API_MICROSOFT_CLIENT_ID})
endif()

if (NOT DEFINED ENV{BETACRAFT_DISCORD_CLIENT_ID})
    set(DISCORD_CLIENT_ID 0)
else()
    set(DISCORD_CLIENT_ID $ENV{BETACRAFT_DISCORD_CLIENT_ID})
endif()

add_compile_definitions(API_MICROSOFT_CLIENT_ID="${API_MICROSOFT_CLIENT_ID}")
add_compile_definitions(DISCORD_CLIENT_ID=${DISCORD_CLIENT_ID})

include(CTest)

add_subdirectory(lib/json-c)
add_subdirectory(lib/cpr)
add_subdirectory(lib/libarchive)
add_subdirectory(lib/gtest)

add_subdirectory(src)
