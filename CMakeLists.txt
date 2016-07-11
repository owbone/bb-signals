project(bb-signals)
cmake_minimum_required(VERSION 3.4)

add_subdirectory(external/googletest)

add_definitions(-std=c++14)

add_subdirectory(include)
add_subdirectory(test)
