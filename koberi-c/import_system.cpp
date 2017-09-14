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

void ImportSystem::parseImports(const std::string & filename) {
    
    std::ifstream file(filename);
    
    if (not file) {
        throw file_not_opened(filename);
    }
    
    std::string line;
    
    while (not file.eof()) {
        
        std::getline(file, line);
        trimStr(line);
        
        /* If line is empty or comment */
        if (not line.size() or line[0] == ';') {
            continue;
        }
        
        if (line[0] == '#') {
            
            std::vector<std::string> split = splitStr(line);
            
        } else {
            
        }
        
    }
    
}

const std::vector<std::string> & ImportSystem::getImportedFiles() {
    
    return _importedFiles;
    
}
