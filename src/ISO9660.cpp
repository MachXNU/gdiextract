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

std::vector<DirectoryRecord> ISO9660::getDirectoryContent(const DirectoryRecord& parent) {
    std::vector<DirectoryRecord> children;

    uint32_t bytesRead = 0;
    uint32_t currentSector = parent.sector;

    while (bytesRead < parent.size) {
        std::vector<uint8_t> sector;
        assert(reader.readSector(currentSector, sector));

        uint32_t offset = 0;
        while (offset < 2048) {
            uint8_t length = sector[offset];

            if (length == 0) {
                // end of sector, jump to next one
                break;
            }

            DirectoryRecord record = parseDirectoryRecord(&sector[offset]);
            children.push_back(record);

            offset += length;
        }
        currentSector++;
        bytesRead += 2048; // count full sector
    }

    return children;
}

void ISO9660::prettyPrintTree(const DirectoryRecord& root, uint16_t depth){
    std::vector<DirectoryRecord> children = getDirectoryContent(root);
    for (auto it = children.begin(); it != children.end(); ++it) {
        if (it->name == "." || it->name == "..")
            continue;
        for (uint16_t i = 0; i < depth-1; i++)
            std::cout << "|   ";
        if (depth > 0)
            std::cout << "|-- ";
        std::cout << it->name << ", content at LBA " << it->sector << ", size of " << (uint32_t)it->size << std::endl;
        if ((*it).flags & (1 << 1)) // is a directory
            prettyPrintTree((*it), depth+1);
    }
}

void ISO9660::extractFile(const DirectoryRecord& fileDirectoryRecord, std::filesystem::path pathToExtractFolder) {
    std::filesystem::path savePath = pathToExtractFolder / fileDirectoryRecord.name;

    if (fileDirectoryRecord.flags & (1 << 1)){
        std::vector<DirectoryRecord> children = getDirectoryContent(fileDirectoryRecord);
        for (auto it = children.begin(); it != children.end(); ++it) {
            if (it->name == "." || it->name == "..")
                continue;
            if (it->flags & (1 << 1)){ // child is a directory
                std::filesystem::create_directory(pathToExtractFolder / it->name);
                extractFile((*it), pathToExtractFolder / it->name);
            } else {
                extractFile((*it), pathToExtractFolder);
            }
        }
    } else { // file contains data        
        std::vector<uint8_t> sectorDump;
        char *fileDump = (char*)malloc(fileDirectoryRecord.size);
        
        uint32_t readSize = 0;
        uint16_t sectorIncrement = 0;
        while (readSize < fileDirectoryRecord.size){
            // This assumes that the file is not fragmented
            // I could be wrong, but I have not seen any Sega GD-ROM 
            // where files are fragmented. 
            // I'll need to find one to implement support

            // std::cout << "File size: " << fileDirectoryRecord.size << std::endl;
            // std::cout << "So far, read " << readSize << std::endl;
            // std::cout << "Reading sector " << fileDirectoryRecord.sector + sectorIncrement << std::endl;
            
            reader.readSector(fileDirectoryRecord.sector + sectorIncrement, sectorDump);
            // std::cout << "Read sector content: ";
            // for (uint8_t b : sectorDump) {
            //     std::cout << std::hex << static_cast<int>(b) << " ";
            // }
            // std::cout << std::dec << std::endl;
            memcpy(fileDump + 2048 * sectorIncrement, sectorDump.data(), std::min((uint32_t)2048, fileDirectoryRecord.size - readSize));
            // std::cout << "Copied " << std::min((uint32_t)2048, fileDirectoryRecord.size - readSize) << std::endl;
            
            // std::cout << "Beginning of fileDump now: " << std::endl;
            // for (int i = 0; i < 100 ; ++i)
            //     std::cout << static_cast<int>(fileDump[i]) << " ";
            // std::cout << std::endl;
            
            readSize += sectorDump.size();
            sectorIncrement += 1;
        }

        // std::cout << "Writing file content: ";
        // for (int i = 0; i < 100 ; ++i)
        //     std::cout << static_cast<int>(fileDump[i]) << " ";
        // std::cout << std::endl;

        std::ofstream file(savePath);
        file.write((char*)fileDump, fileDirectoryRecord.size);
        file.close();

        free(fileDump);
    }
}