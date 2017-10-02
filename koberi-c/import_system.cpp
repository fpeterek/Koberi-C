//
//  import_system.cpp
//  koberi-c
//
//  Created by Filip Peterek on 14/09/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#include "import_system.hpp"
#include <iostream>

void trimFront(std::string & orig) {
    
    if (not orig.size() or isspace(orig[0])) {
        orig = orig.substr(1);
        return trimFront(orig);
    }
    
}

void trimBack(std::string & orig) {
    
    if (not orig.size() or isspace(orig.back())) {
        orig = orig.substr(0, orig.size() - 1);
        return trimBack(orig);
    }
    
}

/* TrimStr() modifies the original string to save stack space */
/* The original string isn't needed anymore, anyway           */
void trimStr(std::string & orig) {
    
    trimFront(orig);
    trimBack(orig);
    
}

std::vector<std::string> splitStr(const std::string & str) {
    
    std::vector<std::string> substrings;
    
    const char delimiter = ' ';
    
    std::stringstream ss(str);
    
    while (not ss.eof()) {
        
        std::string substr;
        std::getline(ss, substr, delimiter);
        
        if (substr.size()) {
            substrings.emplace_back(substr);
        }
        
    }
    
    return substrings;
    
}

bool ImportSystem::isImported(const std::string & filename) {
    
    return contains(_importedFiles, filename);
    
}

void ImportSystem::appendExtensions() {
    
    for (auto & i : _importedFiles) {
        i += syntax::fileExtension;
    }
    
}

void ImportSystem::parseImports(const std::string & filename) {
    
    std::ifstream file(filename + syntax::fileExtension);
    
    if (not file) {
        throw file_not_opened(filename + syntax::fileExtension);
    }
    
    _importedFiles.emplace_back(filename);
    
    std::string line;
    std::vector<std::string> imports;
    
    while (not file.eof()) {
        
        std::getline(file, line);
        
        if (not line.size()) {
            continue;
        }
        
        trimStr(line);
        
        if (line[0] == ';') {
            continue;
        }
        
        if (line[0] == '#') {
            
            /* Trim comments from line */
            line = line.substr(0, line.find(";"));
            
            std::vector<std::string> split = splitStr(line);
            
            if (split[0] != "#import") {
                continue;
            }
            
            for (size_t i = 1; i < split.size(); ++i) {
                if (split[i][0] == ';' or split[i][0] == '#') {
                    break;
                }
                imports.emplace_back(split[i]);
            }
            
        } else {
            break;
        }
        
    }
    
    for (auto & i : imports) {
        if (not isImported(i)) {
            parseImports(i);
        }
    }
    
}

const std::vector<std::string> & ImportSystem::getImportedFiles() {
    
    return _importedFiles;
    
}
