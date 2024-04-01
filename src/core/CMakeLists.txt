file(GLOB CoreSource "*.h" "*.cpp")

add_library(Core
    ${CoreSource}
    ../../lib/discord_game_sdk/discord_game_sdk.h
)

if (WIN32)
    target_link_libraries(Core PUBLIC ${CMAKE_SOURCE_DIR}/lib/discord_game_sdk/x86_64/discord_game_sdk.dll.lib)
elseif (APPLE)
    #set(CMAKE_APPLE_SILICON_PROCESSOR "x86_64")
    set(CMAKE_APPLE_SILICON_PROCESSOR "arm64")

    if(CMAKE_APPLE_SILICON_PROCESSOR STREQUAL "arm64")
        target_link_libraries(Core PUBLIC ${CMAKE_SOURCE_DIR}/lib/discord_game_sdk/x86_64/discord_game_sdk.dylib)
    elseif(CMAKE_APPLE_SILICON_PROCESSOR STREQUAL "x86_64")
        target_link_libraries(Core PUBLIC ${CMAKE_SOURCE_DIR}/lib/discord_game_sdk/x86_64/discord_game_sdk.dylib)
    endif()
elseif (UNIX AND NOT APPLE) # Linux
    add_library(discordgamesdk SHARED IMPORTED)
    set_target_properties(discordgamesdk PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/discord_game_sdk/x86_64/discord_game_sdk.so)
    target_link_libraries(Core PUBLIC discordgamesdk)
endif()

target_link_directories(Core PUBLIC ${CMAKE_SOURCE_DIR}/lib/nlohmann/)

target_link_libraries(Core PUBLIC
    cpr::cpr
    json-c
    archive
)
