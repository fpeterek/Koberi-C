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
#include "contains.hpp"
#include "debug_macro_definitions.h"


class Translator {
    
    
    /* Used for function overloading */
    
    /* Format: Name - Data Type */
    std::unordered_map<std::string, std::string> _functions;  /* Keeps track of functions */
    std::unordered_map<std::string, std::string> _globalVars; /* Keeps track of global variables */
    std::unordered_map<std::string, std::string> _localVars;  /* Keeps track of local variables inside a function. This variable is 
                                                                 reset every time the parseFunc() method is called */
    std::string funCall(); /* Mangle name in a function call so the right function is called */
    
    std::vector<token> & _tokens;
    std::ofstream _output;
    
    void mangleName(std::string & name, std::vector<parameter> & params);
    
    /* parseSexp() parses a s-expression and returns a parameter where type is the data type returned by the s-exp and value
       is the s-exp translated to C */
    parameter parseSexp(unsigned long long sexpBeginning);
    
    void parseDeclarations(); /* Declare functions and global variables */
    void declaration(unsigned long long declBeginning, unsigned long long declEnd);       /* Calls varDeclaration() or funDeclaration(),
                                                                                             exists to increase code readability and 
                                                                                             modularity */
    void varDeclaration(unsigned long long declBeginning, unsigned long long declEnd);    /* Declares a global variable,
                                                                                             called from parseDeclarations() */
    void funDeclaration(unsigned long long declBeginning, unsigned long long declEnd);    /* Declares a function,
                                                                                             called from parseDeclarations() */
    
    void parseFun(unsigned long long funBeginning, unsigned long long funEnd);
    void parseParams(unsigned long long beginning, std::vector<parameter> & params);
    
    /* Outputs includes into the output file */
    void libs();
    /* Outputs typedefs into the output file */
    void typedefs();
    /* Outputs functions into the output file, might change this later */
    void functions();
    
    void parseDefinitions(); /* Parse definitions */
    void definition(unsigned long long defBeginning, unsigned long long defEnd); /* Same as declaration() */
    
public:
    Translator(std::vector<token> & vectorRef);
    
    void translate();
    
};


#endif /* translator_hpp */
