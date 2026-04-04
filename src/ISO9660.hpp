#pragma once
#include "GDISectorReader.hpp"
#include <vector>
#include <string>
#include <optional>

struct DirectoryRecord{
    uint8_t         length;
    uint8_t         xattr_length;
    uint32_t        sector;
    uint32_t        size;
    uint8_t         time[7]; // will take care of that later
    uint8_t         flags;
    uint8_t         unit_size;
    uint8_t         gap_size;
    uint16_t        vol_seq_number;
    uint8_t         name_len;
    std::string     name;
};

class ISO9660 {
public:
    ISO9660(GDISectorReader& reader);
    DirectoryRecord parseDirectoryRecord(const uint8_t* rec);
    std::vector<DirectoryRecord> getDirectoryContent(const DirectoryRecord* parent);

private:
    GDISectorReader& reader;
};