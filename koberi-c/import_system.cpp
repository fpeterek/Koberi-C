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
    
    if (orig.size() and isspace(orig[0])) {
        orig = orig.substr(1);
        return trimFront(orig);
    }
    
}

void trimBack(std::string & orig) {
    
    if (orig.size() and isspace(orig.back())) {
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

void ImportSystem::importType(const std::string & type) {
    
    if (contains(_externTypes, type)) {
        return;
    }
    
    _externTypes.emplace_back(type);
    
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
    std::vector<std::string> types;
    
    while (not file.eof()) {
        
        std::getline(file, line);
        trimStr(line);
        
        if (not line.size()) {
            continue;
        }
        
        if (line[0] == ';') {
            continue;
        }
        
        if (line[0] == '#') {
            
            /* Trim comments from line */
            line = line.substr(0, line.find(";"));
            
            std::vector<std::string> split = splitStr(line);
            
            if (split[0] == "#import") {
                imports = importFiles(split);
            } else if (split[0] == "#extern") {
                types = importTypes(split);
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
    
    for (auto & i : types) {
        importType(i);
    }
    
}

std::vector<std::string> ImportSystem::importFiles(const std::vector<std::string> & files) {
    
    std::vector<std::string> imports;
    
    /* Start at index one because files[0] is #import */
    for (size_t i = 1; i < files.size(); ++i) {
        imports.emplace_back(files[i]);
    }
    
    return imports;
    
}

std::vector<std::string> ImportSystem::importTypes(const std::vector<std::string> & types) {
    
    std::vector<std::string> imports;
    
    /* Start at index one because types[0] is #extern */
    for (size_t i = 1; i < types.size(); ++i) {
        
        if (not syntax::isValidIdChar(types[i].front())) {
            throw bad_type("Error: Cannot import data type " + types[i] + ". ");
        }
        
        for (size_t c = 0; c < types[i].size(); ++c) {
            
            if (not syntax::isValidIdChar(types[i][c])) {
                throw bad_type("Error: Cannot import data type " + types[i] + ". ");
            }
            
        }
        
        imports.emplace_back(types[i]);
        
    }
    
    return imports;
    
}

const std::vector<std::string> & ImportSystem::getImportedFiles() {
    
    return _importedFiles;
    
}

const std::vector<std::string> & ImportSystem::getExternTypes() {
    
    return _externTypes;
    
}
