//
//  koberi-c.cpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#include "koberi-c.hpp"


KoberiC::KoberiC() : _tokenizer(_tokens), _parser(_tokens, _ast), _translator(_ast) {
    
}

void KoberiC::tokenize(const std::string & filename) {
    
    parseInputFileName(filename);
    
    _tokenizer.tokenizeFile(filename);
    
}

void KoberiC::parse() {
    
    _parser.parse();
    
}

void KoberiC::translate() {
    
    _translator.translate();
    
}

void KoberiC::parseInputFileName(const std::string & filename) {
    
    size_t extensionIndex = filename.rfind(".");
    
    _filename = filename.substr(0, extensionIndex) + ".koberice";
    
}


void KoberiC::test() {
    
    int c = 0;
    
    for (auto & i : _tokens) {
        
        print("Token { " + i.value + " }");
        
        if (i.value == "(") ++c;
        if (i.value == ")") --c;
        
        if (not c) { std::endl(std::cout); }
        
    }
    
}
