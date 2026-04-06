cmake_minimum_required(VERSION 3.14)

project(gdiextract)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch argparse from GitHub
include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

add_executable(gdiextract
    src/main.cpp
    src/gdi.cpp
    src/GDISectorReader.cpp
    src/ISO9660.cpp
)

# Link argparse
target_link_libraries(gdiextract PRIVATE argparse)