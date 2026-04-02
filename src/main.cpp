#include <iostream>
#include <cassert>
#include <fstream>

#include "gdi.hpp"

#define MAGIC_SIZE 4

int main(int argc, char* argv[]){
    
    // ========== Argument parsing ==========
    assert(argc >= 2 && "Error, at least 1 argument must be provided");

    bool debug = false;
    std::string filename;

    int i = 0;
    while (i < argc) {
        if (std::string(argv[i]) == "-d")
            debug = true;
        else
            filename = std::string(argv[i]);
        i++;
    }

    // ========== Opening the file ==========
    std::ifstream fileBuffer(filename);
    assert(fileBuffer.is_open() && "File does not exist");

    std::cout << "[+] Opening file " << filename << std::endl;

    // ========== Parsing the .gdi file ==========
    GDIImage img = parseGDI(fileBuffer);

    if (debug){
        std::cout << "[+] Found " << img.tracks.size() << " tracks:" << std::endl;
        for (GDITrack& t : img.tracks)
            std::cout << "[i] " << t.trackNumber << " " << t.filename << "\n";
    }

    fileBuffer.close();
    return 0;
}