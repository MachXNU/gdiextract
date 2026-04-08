#include "gdi.hpp"
#include <sstream>
#include <fstream>
#include <cassert>

GDIImage parseGDI(std::ifstream& file, std::filesystem::path pathOfGdiParentFolder) {
    assert(pathOfGdiParentFolder.is_absolute() && "Requires **absolute** path to GDI file being passed to parseGDI()");
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
        // First, extract the first 4 values which are integers
        iss >> t.trackNumber >> t.lba >> t.trackType >> t.sectorSize;
        
        // Now we need to read the filename (which can contain spaces)
        std::string filename;
        std::getline(iss, filename, '"');  // Skip leading whitespace before filename
        std::getline(iss, filename, '"');  // Read filename until next quote
        
        // Finally extract the offset, which is the last number
        iss >> t.offset;

        // Assign the filename to the struct
        t.filename = filename;
        
        t.absolutePath = pathOfGdiParentFolder / t.filename;

        image.tracks.push_back(t);
    }

    assert(image.tracks.size() == (size_t)expected && "Inconsitent number of tracks in .gdi file");

    assert(image.tracks.at(2).lba == 45000 && "track03 LBA should be 45000");
    return image;
}