cmake_minimum_required(VERSION 3.16)
project(BuckshotRoulette CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)
endif()

# Find SFML 2.6+
find_package(SFML 2.6 COMPONENTS graphics window audio system QUIET)
if(NOT SFML_FOUND)
    find_package(SFML 2.5 COMPONENTS graphics window audio system QUIET)
endif()
if(NOT SFML_FOUND)
    message(FATAL_ERROR "SFML not found. Install libsfml-dev (Ubuntu/Debian) or sfml (Arch/Homebrew).")
endif()

message(STATUS "Found SFML ${SFML_VERSION}")

set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/DealerAI.cpp
    src/Item.cpp
    src/Particle.cpp
    src/Shell.cpp
    src/Shotgun.cpp
    src/SoundManager.cpp
    src/UI.cpp
    src/Utils.cpp
)

add_executable(BuckshotRoulette ${SOURCES})

target_include_directories(BuckshotRoulette PRIVATE include)

target_link_libraries(BuckshotRoulette
    sfml-graphics
    sfml-window
    sfml-audio
    sfml-system
)

if(UNIX AND NOT APPLE)
    target_link_libraries(BuckshotRoulette pthread)
endif()

# Copy assets if they exist
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
endif()

install(TARGETS BuckshotRoulette DESTINATION bin)
