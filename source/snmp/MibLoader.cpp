/**
 * @file MibLoader.cpp
 * @brief MIB loader
 */

// Includes C/C++
#include <fstream>
#include <sstream>

// Own includes
#include "snmp/MibLoader.h"

namespace NetMan {

MibLoader::MibLoader() { }

std::shared_ptr<Mib> MibLoader::load(const std::string &path) {

    // Open file
    FILE *f = fopen(path.c_str(), "rb");
    if(f == NULL) {
        throw std::runtime_error(path + " can't be opened");
    }

    // Read the entire file
    while(!feof(f)) {
        skipUntilReadable(f);
    }

    // Close file
    fclose(f);

    return nullptr;
}

void MibLoader::skipUntilReadable(FILE *f) {
    bool readable = false;
    while(!readable) {
        char c = fgetc(f);

        // Skip comments
        if(c == '-') {
            if(fgetc(f) == '-') {
                while(fgetc(f) != '\n');
            } else {
                fseek(f, -1, SEEK_CUR);
            }
        } else if(c != ' ' && c != '\t' && c != '\n' && c != '\r') {    // Remove fillers
            fseek(f, -1, SEEK_CUR);
            readable = true;
        }
    }
}

MibLoader::~MibLoader() { }

}
