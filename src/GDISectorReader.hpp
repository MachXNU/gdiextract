#pragma once
#include "gdi.hpp"
#include <fstream>
#include <vector>
#include <cassert>
#include <iostream>

#define HIGH_DENSITY_SECTOR_START 45000

class GDISectorReader{
public:
    // Constructor
    // the GDISectorReader will read the sectors of a specific GDIImage
    GDISectorReader(const GDIImage& image);

    // returns true if success
    bool readSector(uint32_t diskLBA, std::vector<uint8_t>& out);

private:
    const GDITrack* isoTrack;    // track03 with the ISO9660 header (to confirm)
    const GDITrack* dataTrack;   // last track if multi-track
    bool singleTrack;            // only true for 3 tracks GD-ROMs
    std::ifstream isoFile;
    std::ifstream dataFile;
    uint32_t isoTrackSectors;    // number of sectors in track03
};