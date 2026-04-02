#pragma once
#include <string>
#include <vector>

struct GDITrack {
    int trackNumber;
    int lba;
    int trackType;
    int sectorSize;
    std::string filename;
    int offset;
};

struct GDIImage {
    std::vector<GDITrack> tracks;
};

GDIImage parseGDI(std::ifstream& file);