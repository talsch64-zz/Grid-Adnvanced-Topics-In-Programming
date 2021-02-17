cmake_minimum_required(VERSION 3.17)
project(Grid_ex4_skeleton_main)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

set(CMAKE_CXX_FLAGS "-std=c++20 -Werror -Wall -Wextra -pedantic-errors -DNDEBUG")

add_executable(Grid_ex4_skeleton_main
        CoordinatesMath.h
        Double.h
        GISNamedTypes.h
        main.cpp
        NamedType.h
        tests.cpp Grid.h GridMock.h TestClasses.h)
target_link_libraries(Grid_ex4_skeleton_main gtest gtest_main pthread)
