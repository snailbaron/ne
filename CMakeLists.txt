cmake_minimum_required(VERSION 3.0)

project (ne)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_COLOR_DIAGNOSTICS TRUE)

add_library(ne
    src/bencoding.cpp
    src/element.cpp
    src/http.cpp
    src/io.cpp
    src/metainfo.cpp
    src/tcp_connection.cpp
    src/tcp_listen.cpp
    src/tcp_stream.cpp
    src/tcp_streambuf.cpp
    src/url.cpp
)
target_include_directories(ne PUBLIC include)

add_subdirectory(tools)
