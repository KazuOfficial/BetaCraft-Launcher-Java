if (BUILD_TESTING)
    add_executable(BetacraftTests test.cpp)
    target_link_libraries(BetacraftTests PRIVATE GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(BetacraftTests)
endif()
