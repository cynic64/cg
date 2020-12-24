cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)

project(cg)

find_package(Threads REQUIRED)

add_executable(cg cg.cpp)

target_link_libraries(cg PRIVATE Threads::Threads)
