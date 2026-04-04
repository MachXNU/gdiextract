#include "ISO9660.hpp"
#include "NumericalFormats.hpp"

ISO9660::ISO9660(GDISectorReader& _reader)
    : reader(_reader)
{
}

DirectoryRecord ISO9660::parseDirectoryRecord(const uint8_t* record){
    DirectoryRecord r{};

    r.length       = record[0];
    r.xattr_length = record[1];

    r.sector = readLE32(&record[2]);
    r.size   = readLE32(&record[10]);

    std::memcpy(r.time, record + 18, 7);

    r.flags     = record[25];
    r.unit_size = record[26];
    r.gap_size  = record[27];

    r.vol_seq_number = readLE16(&record[28]);

    r.name_len = record[32];

    const char* namePtr = (const char*)(record + 33);
    r.name.assign(namePtr, r.name_len); // this converts a char* into a std::string

    // ---- Special cases ----
    if (r.name_len == 1) {
        if ((uint8_t)namePtr[0] == 0) r.name = ".";
        else if ((uint8_t)namePtr[0] == 1) r.name = "..";
    }

    // ---- Remove ;1 version ----
    size_t semi = r.name.find(';');
    if (semi != std::string::npos)
        r.name = r.name.substr(0, semi);
    
    return r; 
}

std::vector<DirectoryRecord> ISO9660::getDirectoryContent(const DirectoryRecord* parent){
    std::cout << "Directory content size: " << parent->size << std::endl;
    
    std::vector<DirectoryRecord> children;

    std::vector<uint8_t> directorySector;
    assert(reader.readSector(parent->sector, directorySector) && "Error, failed to read content directory sector");

    uint16_t offset = 0;
    while (directorySector[offset] != 0) {
        DirectoryRecord content = parseDirectoryRecord(&directorySector[offset]);
        children.push_back(content);
        offset += content.length;
    }

    return children;
}