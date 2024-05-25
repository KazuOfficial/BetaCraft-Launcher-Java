file(GLOB BetacraftLauncherCoreSources "*.cpp" "*.hpp")
add_library(BetacraftLauncherCore ${BetacraftLauncherCoreSources})

add_subdirectory(tests)
