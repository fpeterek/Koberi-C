//
//  translator.hpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#ifndef translator_hpp
#define translator_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <cmath>
#include <fstream>
#include <sstream>

#include "syntax.hpp"
#include "token.hpp"
#include "exceptions.hpp"
#include "print.hpp"
#include "expressions.hpp"
#include "debug_macro_definitions.h"

struct parameter {
    
    std::string type;
    std::string value;
    
};

class Translator {
    
    std::vector<token> & _tokens;
    std::ofstream _output;
    
    void mangleName(std::string & name, std::vector<parameter> & params);
    
    std::string parseSexp(unsigned long long sexpStart);
    
    void parseFunc(unsigned long long funcStart, unsigned long long funcEnd);
    void parseParams(unsigned long long start, std::vector<parameter> & params);
    
public:
    Translator(std::vector<token> & vectorRef);
    
    void translate();
    
};


#endif /* translator_hpp */
