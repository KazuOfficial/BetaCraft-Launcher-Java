file(GLOB BetacraftLauncherCoreSources
    "*.cpp"
    "*.hpp"
    "instances/*.cpp"
    "instances/*.hpp")
add_library(BetacraftLauncherCore ${BetacraftLauncherCoreSources})

add_subdirectory(tests)

target_include_directories(BetacraftLauncherCore PRIVATE ${CMAKE_SOURCE_DIR}/externals/nlohmann)

if(UNIX AND NOT APPLE)
    target_link_libraries(BetacraftLauncherCore PRIVATE uuid)
endif()
