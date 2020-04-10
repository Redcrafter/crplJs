cmake_minimum_required (VERSION 2.6)
project(CRPL C CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE Common_sources "./src/*cpp")

add_executable(crpl ${Common_sources})
target_include_directories(crpl PRIVATE "./src")

if(UNIX)
    target_link_libraries(crpl stdc++fs)
endif()
