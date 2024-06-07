file(GLOB BetacraftLauncherCoreTestsSources 
    "*.cpp"
    "instances/*.cpp")
add_executable(BetacraftLauncherCoreTests ${BetacraftLauncherCoreTestsSources})

include(GoogleTest)
gtest_discover_tests(BetacraftLauncherCoreTests)

target_include_directories(BetacraftLauncherCoreTests PRIVATE ${CMAKE_SOURCE_DIR}/externals/nlohmann)

target_link_libraries(BetacraftLauncherCoreTests PRIVATE 
    GTest::gtest_main 
    BetacraftLauncherCore
)
