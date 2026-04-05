cmake_minimum_required(VERSION 3.14)

project(gdiextract)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(gdiextract
    src/main.cpp
    src/gdi.cpp
    src/GDISectorReader.cpp
    src/ISO9660.cpp
)