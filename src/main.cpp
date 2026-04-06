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
    // assert(argc >= 2 && "Error, at least 1 argument must be provided");

    bool debug = false;

    argparse::ArgumentParser program("gdiextract");

    program.add_argument("input");

    program.add_argument("-o", "--output")
        .default_value(std::string("extracted"))
        .required()
        .help("specify the output directory (to save the extracted files)");

    program.add_argument("-d", "--debug")
        .flag()
        .help("print more debug information");

    program.add_argument("-l", "--list")
        .flag()
        .help("tree-list GD-ROM content without extracting");
    
    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    std::filesystem::path filename(program.get<std::string>("input"));

    // ========== Opening the file ==========
    std::ifstream fileBuffer(filename);
    if (program["--debug"] == true)
        std::cout << "[i] Opening file " << filename << std::endl;
    assert(fileBuffer.is_open() && "File does not exist");


    // ========== Parsing the .gdi file ==========
    GDIImage img = parseGDI(fileBuffer, filename.parent_path());

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
        assert(sig == "CD001" && "PVD seems wrong, did not read CD001 as expected");
        if (program["--debug"] == true)
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

    if (program["--list"] == true){
        fs.prettyPrintTree(rootDirectoryRecord);
    } else {
        // ========== Extracting the content of the root directory ==========
        std::filesystem::path saveDirectory = program.get<std::string>("--output");
        std::filesystem::create_directory(saveDirectory);
        fs.extractFile(rootDirectoryRecord, saveDirectory);
            std::cout << "Extracted files to " << saveDirectory << std::endl;
    }

    // ========== Cleanup ==========
    fileBuffer.close();

    return 0;
}