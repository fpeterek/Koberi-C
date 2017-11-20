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

std::vector<std::string> splitImport(const std::string & str) {
    
    std::vector<std::string> substrings;
    
    const char delimiter = ' ';
    
    std::stringstream ss(str);
    
    while (not ss.eof()) {
        
        std::string substr;
        std::getline(ss, substr, delimiter);
        
        if (substrings.size() and substrings.back().back() != '"' ) {
            /* Delimiter " " was removed, put it back */
            substrings.back() += " " + substr;
        }
        
        else if (substr.size()) {
            substrings.emplace_back(substr);
        }
        
    }
    
    /* Trim quotes */
    for (auto & i : substrings) {
        
        /* Check for missing " */
        if (i.back() != '"' or i.front() != '"') {
            throw missing_token('"');
        }
        
        i = i.substr(1);
        i.pop_back();
    }
    
    return substrings;
    
}

std::vector<std::string> splitExtern(const std::string & str) {
    
    std::vector<std::string> substrings;
    
    std::string substr;
    
    for (size_t i = 0; i < str.size(); ++i) {
        
        if (str[i] == '<' or str[i] == '"') {
            
            const char endChar = str[i] == '<' ? '>' : '"';
            const size_t start = i;
            
            while (true) {
                
                if (++i == str.size()) {
                    throw missing_token(endChar);
                }
                
                if (str[i] == endChar) {
                    break;
                }
                
            }
            
            substrings.emplace_back(str.substr(start, i - start + 1));
            
        }
        else if (isspace(str[i])) {
            continue;
        }
        else {
            
            const size_t start = i;
            
            while (++i < str.size()) {
                if (isspace(str[i])) {
                    break;
                }
            }
            
            substrings.emplace_back(str.substr(start, i - start));
            
        }
        
    }
    
    return substrings;
    
}

bool ImportSystem::isImported(const std::string & filename) {
    
    return contains(_importedFiles, filename);
    
}

void ImportSystem::importLib(const std::string & lib) {
    importInto(_cLibs, lib);
}

void ImportSystem::importType(const std::string & type) {
    importInto(_externTypes, type);
}

void ImportSystem::importInto(std::vector<std::string> & collection, const std::string & value) {
    
    if (contains(collection, value)) {
        return;
    }
    
    collection.emplace_back(value);
    
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
    std::vector<std::string> stdLibs;
    std::vector<std::string> libs;
    
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
            
            std::string statement = line.substr(0, line.find(" "));
            line = line.substr(line.find(" "));
            
            if (statement == "#import") {
                std::vector<std::string> split = splitImport(line);
                imports = importFiles(split);
            } else if (statement == "#extern") {
                std::vector<std::string> split = splitExtern(line);
                types = importTypes(split);
                stdLibs = importLibraries(split, true);
                libs = importLibraries(split);
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
    for (auto & i : stdLibs) {
        importLib(i);
    }
    for (auto & i : libs) {
        importLib(i);
    }
}

std::vector<std::string> ImportSystem::importFiles(const std::vector<std::string> & files) {
    
    std::vector<std::string> imports;
    
    for (size_t i = 0; i < files.size(); ++i) {
        imports.emplace_back(files[i]);
    }
    
    return imports;
    
}

std::vector<std::string> ImportSystem::importTypes(const std::vector<std::string> & types) {
    
    std::vector<std::string> imports;
    
    for (size_t i = 0; i < types.size(); ++i) {
        
        if (types[i].front() == '"' or types[i].front() == '<') {
            continue;
        }
        
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

std::vector<std::string> ImportSystem::importLibraries(const std::vector<std::string> & libs, bool isStdLib) {
    
    std::vector<std::string> libraries;
    
    const char endChar = isStdLib ? '>' : '"';
    
    for (auto & i : libs) {
        if (i.back() == endChar) {
            libraries.emplace_back(i);
        }
    }
    
    return libraries;
    
}

const std::vector<std::string> & ImportSystem::getImportedFiles() {
    
    return _importedFiles;
    
}

const std::vector<std::string> & ImportSystem::getExternTypes() {
    
    return _externTypes;
    
}

const std::vector<std::string> & ImportSystem::getCLibs() {
    
    return _cLibs;
    
}
