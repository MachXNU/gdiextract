#pragma once
#include <string>
#include <vector>

struct GDITrack {
    int trackNumber;
    uint32_t lba;
    int trackType;
    uint32_t sectorSize;
    std::string filename;
    int offset;
};

struct GDIImage {
    std::vector<GDITrack> tracks;
};

GDIImage parseGDI(std::ifstream& file);