cmake_minimum_required(VERSION 3.5)
project(discipline_graphics LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

file(GLOB PAGES "src/pages/*.cpp")

set(SOURCES src/main.cpp src/components.cpp src/audio-math-utils.cpp ${PAGES})

if (NOT WIN32)
    list(APPEND SOURCES src/audio-sink-jack.cpp src/osc-server.cpp)
endif()    

include_directories(lib)
link_directories(lib)

add_executable(discipline_graphics ${SOURCES})

target_link_libraries(discipline_graphics raylib)

if (WIN32)
    target_link_libraries(discipline_graphics gdi32 winmm)
else()
    target_link_libraries(discipline_graphics lo jack)
endif()