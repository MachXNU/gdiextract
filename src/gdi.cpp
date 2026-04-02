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

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        GDITrack t;

        // streams the string, stopping at each whitespace until stream ends
        iss >> t.trackNumber >> t.lba >> t.trackType
            >> t.sectorSize >> t.filename >> t.offset;

        image.tracks.push_back(t);
    }

    assert(image.tracks.size() == (size_t)expected && "Inconsitent number of tracks in .gdi file");

    return image;
}