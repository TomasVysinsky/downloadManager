cmake_minimum_required(VERSION 3.18)
project(downloadManager C)

set(CMAKE_C_STANDARD 99)

add_executable(downloadManager main.c client/client.h client/client.c server/server.h server/server.c clientServer/client.c clientServer/server.c)

target_link_libraries(downloadManager pthread)