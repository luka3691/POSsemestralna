cmake_minimum_required(VERSION 3.18)
project(asynchklientserver CXX)

set(CMAKE_CXX_STANDARD 11)

add_executable(asynchklientserver-klient dataDef.h dataDef.cpp klient.h klient.cpp)
target_link_libraries(asynchklientserver-klient pthread)

add_executable(asynchklientserver-server dataDef.cpp dataDef.h klient.cpp klient.h)
target_link_libraries(asynchklientserver-server pthread)
