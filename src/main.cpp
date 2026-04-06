#include <iostream>
#include <cassert>
#include <fstream>

#include <argparse/argparse.hpp>

#include "gdi.hpp"
#include "GDISectorReader.hpp"
#include "ISO9660.hpp"

#define MAGIC_SIZE 4

int main(int argc, char* argv[]){
    
    // ========== Argument parsing ==========
    assert(argc >= 2 && "Error, at least 1 argument must be provided");

    bool debug = false;
    std::string filename;

    argparse::ArgumentParser program("gdiextract");

    program.add_argument("input");

    program.add_argument("-o", "--output")
        .default_value(std::string("extracted"))
        .required()
        .help("specify the output directory (to save the extracted files)");

    program.add_argument("-d", "--debug")
        .implicit_value(true)
        .help("print more debug information");
    
    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    filename = program.get<std::string>("input");

    // ========== Opening the file ==========
    std::ifstream fileBuffer(filename);
    assert(fileBuffer.is_open() && "File does not exist");

    std::cout << "[+] Opening file " << filename << std::endl;

    // ========== Parsing the .gdi file ==========
    GDIImage img = parseGDI(fileBuffer);

    if (program["--debug"] == true){
        std::cout << "[+] Found " << img.tracks.size() << " tracks:" << std::endl;
        for (GDITrack& t : img.tracks)
            std::cout << "[i] " << t.trackNumber << " " << t.filename << "\n";
    }

    GDISectorReader reader(img);

    // ========== Reading the Primary Volume Directory ==========

    std::vector<uint8_t> sector;
    if (reader.readSector(45016, sector)) {
        // Should contain Primary Volume Descriptor
        std::string sig(reinterpret_cast<char*>(sector.data() + 1), 5);
        std::cout << "PVD signature: " << sig << "\n"; // should be "CD001"
    }

    // ========== Reading the Root Directory Record ==========

    const uint8_t* root = &sector[156];

    ISO9660 fs(reader);

    DirectoryRecord rootDirectoryRecord = fs.parseDirectoryRecord(root);

    if (program["--debug"] == true){
        std::cout << "Checking root directory record:" << std::endl;
        std::cout << "Directory name: " << rootDirectoryRecord.name << std::endl;
        std::cout << "Sector: " << rootDirectoryRecord.sector << std::endl;
        std::cout << "====================" << std::endl;
    }

    // ========== Reading the content of the root directory ==========

    std::vector<uint8_t> rootContentSector;
    assert(reader.readSector(rootDirectoryRecord.sector, rootContentSector) && "Error, failed to read root content directory sector");

    std::vector<DirectoryRecord> content = fs.getDirectoryContent(rootDirectoryRecord);

    fs.prettyPrintTree(rootDirectoryRecord);

    // ========== Cleanup ==========
    fileBuffer.close();

    return 0;
}