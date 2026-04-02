#include "gdi.hpp"
#include <sstream>
#include <fstream>
#include <cassert>

GDIImage parseGDI(std::ifstream& file) {
    GDIImage image;
    std::string line;

    // First line stores the number of tracks
    std::getline(file, line);
    int expected = std::stoi(line);

    assert(expected >= 3 && "There should be at least 3 tracks in a valid .gdi file");

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        GDITrack t;

        // streams the string, stopping at each whitespace until stream ends
        iss >> t.trackNumber >> t.lba >> t.trackType
            >> t.sectorSize >> t.filename >> t.offset;

        image.tracks.push_back(t);
    }

    assert(image.tracks.size() == (size_t)expected && "Inconsitent number of tracks in .gdi file");

    assert(image.tracks.at(2).lba == 45000 && "track03 LBA should be 45000");
    return image;
}