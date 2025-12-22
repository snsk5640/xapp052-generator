cmake_minimum_required(VERSION 3.10)
project(lfsr-generator VERSION 1.0 LANGUAGES CXX)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source and include directories
include_directories(include)

# Executable name and source files
add_executable(lfsr-generator 
    src/main.cpp
    src/lfsr_core.cpp
    src/app_modes.cpp
)

# Copy the taps.csv to the build directory so the app can find it
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/taps.csv DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)
