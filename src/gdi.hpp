#pragma once
#include <string>
#include <vector>
#include <fstream>

struct GDITrack {
    int trackNumber;
    uint32_t lba;
    int trackType;
    uint32_t sectorSize;
    std::string filename;
    std::filesystem::path absolutePath;
    int offset;
};

struct GDIImage {
    std::vector<GDITrack> tracks;
};

GDIImage parseGDI(std::ifstream& file, std::filesystem::path pathOfGdiParentFolder);