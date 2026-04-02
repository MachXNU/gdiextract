#pragma once
#include "gdi.hpp"
#include <fstream>
#include <vector>
#include <cassert>
#include <iostream> // for std::cout

class GDISectorReader{
public:
    // Constructor
    // the GDISectorReader will read the sectors of a specific GDIImage
    GDISectorReader(const GDIImage& image);

    // returns true if success
    bool readSector(int lba, std::vector<uint8_t>& out);

private:
    const GDITrack* isoTrack;    // track03 with the ISO9660 header (to confirm)
    const GDITrack* dataTrack;   // last track if multi-track
    bool singleTrack;            // only true for 3 tracks GD-ROMs
    std::ifstream isoFile;
    std::ifstream dataFile;
    int isoTrackSectors;         // number of sectors in track03
};