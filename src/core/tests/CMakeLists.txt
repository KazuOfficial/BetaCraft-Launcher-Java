file(GLOB BetacraftLauncherCoreTestsSources "*.cpp")
add_executable(BetacraftLauncherCoreTests ${BetacraftLauncherCoreTestsSources})

include(GoogleTest)
gtest_discover_tests(BetacraftLauncherCoreTests)

target_link_libraries(BetacraftLauncherCoreTests PRIVATE 
    GTest::gtest_main 
    BetacraftLauncherCore
)
