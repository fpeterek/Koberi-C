//
//  tokenizer.hpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#ifndef tokenizer_hpp
#define tokenizer_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
#include <fstream>

#include "syntax.hpp"
#include "print.hpp"
#include "exceptions.hpp"
#include "token.hpp"

#include "debug_macro_definitions.hpp"


class Tokenizer {
    
    std::vector<token> & _tokens; /* Reference to a vector of tokens */
    
    // std::string _filename;
    
    std::ifstream _input;
    std::string _line;
    size_t _lineLen;
    size_t _iter;
    
    void identifierCheck();
    void operatorCheck();
    void numberLiteral();
    void strLiteral(const char delimiter='"');
    void readLine();
    void parseLine();
    void openFile(const std::string & filename);
    
public:
    
    Tokenizer(std::vector<token> & vectorRef);
    void tokenizeFile(const std::string & filename);
    
};


#endif /* tokenizer_hpp */
