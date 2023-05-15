cmake_minimum_required(VERSION 3.0)

project(hehe)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_COLOR_DIAGNOSTICS TRUE)

add_library(hehe
    src/hehe.cpp
    src/socket.cpp
)
target_include_directories(hehe PUBLIC include)

add_subdirectory(tools)
