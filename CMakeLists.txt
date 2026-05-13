cmake_minimum_required(VERSION 3.14)

project(gdiextract)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(argparse REQUIRED)

add_executable(gdiextract
    src/main.cpp
    src/gdi.cpp
    src/GDISectorReader.cpp
    src/ISO9660.cpp
)

# Link argparse
target_link_libraries(gdiextract PRIVATE argparse::argparse)

install(TARGETS gdiextract DESTINATION bin)
