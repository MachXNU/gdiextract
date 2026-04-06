#include "GDISectorReader.hpp"

GDISectorReader::GDISectorReader(const GDIImage& image){
    isoTrack = &image.tracks[2];  // track03
    singleTrack = (image.tracks.size() == 3);
    if (!singleTrack)
        dataTrack = &image.tracks.back(); // last track

    // TODO: store absolute path instead of relative path
    isoFile.open(isoTrack->absolutePath, std::ios::binary);
    assert(isoFile.is_open() && "Failed to open isoFile");

    if (!singleTrack){
        dataFile.open(dataTrack->absolutePath, std::ios::binary);
        assert(dataFile.is_open() && "Failed to open dataFile");
    }

    // compute number of sectors in track 3
    isoFile.seekg(0, std::ios::end);
    isoTrackSectors = static_cast<int>(isoFile.tellg() / isoTrack->sectorSize);
    std::cout << "[i] Number of sectors in track03: " << isoTrackSectors << std::endl;
    isoFile.seekg(0, std::ios::beg);
}

bool GDISectorReader::readSector(uint32_t diskLBA, std::vector<uint8_t>& out){
    // This is a GD-ROM sector!
    // To read a ISO9660 FS sector, add 45000 to the input.
    // For example, to read the PVD (FS sector 16), input diskLBA = 45016
    const int RAW = 2352; // TODO: extract this from GDITrack.sectorSize instead?
    const int OFFSET = 16; // strip 16 bytes before reading a real sector

    std::ifstream* file;
    uint32_t fsLBA = diskLBA - HIGH_DENSITY_SECTOR_START;
    uint32_t fileLBA = fsLBA;

    if (singleTrack){
        file = &isoFile;
    } else {
        if (fsLBA < isoTrackSectors){ // we are reading a sector inside the ISO9660 header
            file = &isoFile;
        } else { // we have to seek to the dataFile
            file = &dataFile;
            // track03 already contains isoTrackSectors sectors
            // so the next one lives in the dataTrack, and we should remove
            // the number of sectors stored in track03 already
            fileLBA = fsLBA - isoTrackSectors;
        }
    }

    out.resize(2048);
    file->seekg(fileLBA * RAW + OFFSET);
    file->read(reinterpret_cast<char*>(out.data()), 2048);

    return file->good(); // returns true if all the stream flags are false
}