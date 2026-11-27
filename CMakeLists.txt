cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(ChaosJump VERSION 1.0.4)

include(FetchContent)

add_library(${PROJECT_NAME} SHARED
    src/main.cpp
)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "GEODE_SDK environment variable is not set")
endif()

message(STATUS "Using Geode SDK: $ENV{GEODE_SDK}")
add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# Game Objects Factory is required at runtime, but it is not available in the
# Geode index used by CI. Fetch its public 1.1.2 headers to compile our custom
# trigger registrations, and declare the mod dependency as external to Geode's
# dependency resolver.
FetchContent_Declare(
    game_objects_factory
    URL https://github.com/lil2kki/geode-game-objects-factory/archive/refs/tags/1.1.2.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_GetProperties(game_objects_factory)
if (NOT game_objects_factory_POPULATED)
    FetchContent_Populate(game_objects_factory)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${game_objects_factory_SOURCE_DIR}/include
)

setup_geode_mod(${PROJECT_NAME} EXTERNALS user95401.game-objects-factory:>=1.1.2)
