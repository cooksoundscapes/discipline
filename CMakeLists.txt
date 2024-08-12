cmake_minimum_required(VERSION 3.5)
project(discipline LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(raylib REQUIRED)

set(SOURCES src/main.cpp)

include_directories(lib)

add_executable(discipline ${SOURCES})

target_link_libraries(discipline raylib lo lua)