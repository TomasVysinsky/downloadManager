cmake_minimum_required(VERSION 3.18)
project(downloadManager C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS -lcurl)

add_executable(downloadManager downloadManager.c Makefile)

find_package(CURL REQUIRED)
target_link_libraries(downloadManager CURL::libcurl pthread)