//
//  koberi-c.cpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#include "koberi-c.hpp"


KoberiC::KoberiC() : _tokenizer(_tokens), _parser(_tokens) /* Call the constructors which accept references */ {
    
}

void KoberiC::tokenize(const std::string & filename) {
    
    _tokenizer.tokenizeFile(filename);
    
}

void KoberiC::parse() {
    
    _parser.parse();
    
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
