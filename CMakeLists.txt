cmake_minimum_required(VERSION 3.14)
project(gdiextract)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT DEFINED argparse_FOUND)
    # Try to find system or Nix-provided argparse first
    find_package(argparse QUIET)
endif()

if(NOT argparse_FOUND)
    # fallback: download via FetchContent
    include(FetchContent)
    message(STATUS "argparse not found, fetching from GitHub...")

    FetchContent_Declare(
        argparse
        GIT_REPOSITORY https://github.com/p-ranav/argparse.git
        GIT_TAG        master
    )
    FetchContent_MakeAvailable(argparse)
endif()

add_executable(gdiextract
    src/main.cpp
    src/gdi.cpp
    src/GDISectorReader.cpp
    src/ISO9660.cpp
)

target_link_libraries(gdiextract PRIVATE argparse::argparse)

install(TARGETS gdiextract DESTINATION bin)
