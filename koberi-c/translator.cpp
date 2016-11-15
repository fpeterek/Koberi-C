//
//  translator.cpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#include "translator.hpp"

Translator::Translator(std::vector<token> & vectorRef) : _tokens(vectorRef) /* Call the reference constructor */ {
    
    _output.open("output.c");
    
    if (not _output) {
        
        throw file_not_opened("output.c");
        
    }
    
}

void Translator::mangleName(std::string & name, std::vector<parameter> & params) {
    
    for (auto & i : params) {
        
        name += "_" + i.type.substr(0, 1);
        
    }
    
}

void Translator::parseParams(unsigned long long start, std::vector<parameter> & params) {
    
    for (unsigned long long i = start; _tokens[i] != tokType::closingPar; i += 2) {
        
        if (_tokens[i] == tokType::closingPar or _tokens[i+1] == tokType::closingPar) { break; }
        params.emplace_back();
        params.back().type = _tokens[i].value;
        params.back().value = _tokens[i+1].value;
        
    }
    
}

void Translator::parseFunc(unsigned long long funcStart, unsigned long long funcEnd) {
    
#if PRINT_FUNCTIONS_TOKENS
    for (unsigned long long i = funcStart; i < funcEnd; ++i) {
        print(_tokens[i].value);
    }
#endif
    
    std::string type = _tokens[1].value;
    std::string name = _tokens[2].value;
    std::vector<parameter> params;
    
    parseParams(funcStart + 3, params);
    mangleName(name, params);
    std::cout << type << " " << name << "(";
    for (auto i : params) {
        
        std::cout << i.type << " " << i.value << ",";
        
    }
    std::cout << ")";
    
}

void Translator::translate() {
    
    unsigned long long parens = 0;
    unsigned long long funcStart = 0, funcEnd = 0;
    
    for (int i = 0; i < _tokens.size(); ++i) {
        
        if (_tokens[i].type == tokType::openingPar) {
            
            ++parens;
            if (parens == 1) { funcStart = i; }
            
        }
        else if (_tokens[i].type == tokType::closingPar) {
            
            --parens;
            if (not parens) { funcEnd = i; }
            parseFunc(funcStart, funcEnd);
            
        }
        
    }
    
}
