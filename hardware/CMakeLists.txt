cmake_minimum_required(VERSION 3.5)
project(discipline_hardware LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(SOURCES src/main.cpp src/aux-functions.cpp ${PAGES})

include_directories(lib)
link_directories(lib)

add_executable(discipline_hardware ${SOURCES})

target_link_libraries(discipline_hardware i2c lo pigpiod_if2)