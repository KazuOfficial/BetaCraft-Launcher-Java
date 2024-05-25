cmake_minimum_required(VERSION 3.8)

project(BetacraftLauncher LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()


add_subdirectory(externals/googletest)

add_subdirectory(src)
