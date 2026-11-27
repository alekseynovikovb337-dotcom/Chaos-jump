cmake_minimum_required(VERSION 3.21)
project(ChaosJump VERSION 1.0.0)

# Build with the Geode SDK/toolchain for Geometry Dash 2.2081.
add_library(${PROJECT_NAME} SHARED
    src/main.cpp
)

setup_geode_mod(${PROJECT_NAME})
