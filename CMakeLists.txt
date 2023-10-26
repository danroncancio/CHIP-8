cmake_minimum_required(VERSION 3.25)
project(Chip8)

set(CMAKE_CXX_STANDARD 20)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Debug build")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

add_subdirectory(libs/raylib)

add_executable(Chip8 src/main.cpp src/chip8.cpp)

target_include_directories(Chip8 PUBLIC libs/raylib/src)

if (UNIX)
    message(STATUS "Linux platform")
    target_link_libraries(Chip8 PUBLIC raylib)
endif (UNIX)

if (WIN32)
    message(STATUS "Windows platform")
    target_link_libraries(Chip8 PUBLIC raylib)
    target_link_libraries(Chip8 PUBLIC gdi32 opengl32 imm32)
endif(WIN32)
