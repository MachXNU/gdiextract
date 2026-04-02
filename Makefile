CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
SRC = src/main.cpp src/gdi.cpp src/GDISectorReader.cpp
OUT = build/gdiextract

all:
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

clean:
	rm -rf build