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
#include <list>
#include <unordered_map>
#include <map>
#include <cmath>
#include <fstream>
#include <sstream>
#include <functional>

#include "syntax.hpp"
#include "token.hpp"
#include "exceptions.hpp"
#include "print.hpp"
#include "expressions.hpp"
#include "contains.hpp"
#include "debug_macro_definitions.h"
#include "class.hpp"
#include "ast.hpp"


class Parser {
    
    AbstractSyntaxTree & _ast;
    
    std::vector<std::string> dataTypes = {"int", "num", "str", "void"};
    std::unordered_map<std::string, _class> classes; /* Keeps track of classes and their attributes / methods */
    
    void checkType(std::string & type);
    
    /* Used for function overloading */
    
    /* Format: Name - Data Type */
    std::unordered_map<std::string, std::string> _functions;  /* Keeps track of functions */
    std::unordered_map<std::string, std::string> _globalVars; /* Keeps track of global variables */
    std::unordered_map<std::string, std::string> _localVars;  /* Keeps track of local variables inside a function. This variable is 
                                                                 reset every time the parseFunc() method is called */
    
    /* --------------------------------------------------------------------------------------------------------- */
    /*                                                                                                           */
    /* std::unordered_map<std::string, _class> _classes; // Keeping this here as a reminder of my stupid mistake */
    /*                                                                                                           */
    /* --------------------------------------------------------------------------------------------------------- */
    
    std::string funCall(); /* Mangle name in a function call so the right function is called */
    
    std::vector<token> & _tokens;
    std::ofstream _output;
    
    void mangleName(std::string & name, std::vector<parameter> & params);
    
    /* Gets type of a variable or literal */
    std::string getType(token & tok);
    std::string getVarType(parameter & param);
    std::string getVarType(std::string & varName);
    
    /* parseSexp() parses an s-expression and returns a parameter where type is the data type returned by the s-exp and value
       is the s-exp translated to C */
    parameter parseSexp(unsigned long long sexpBeginning);
    
    /* Similar to parseSexps(), but adjusted to work properly on classes */
    std::vector<parameter> parseClassMembers(unsigned long long firstSexp, std::string & className);
    /* Similar to parseSexp(), but only parses class attributes */
    parameter parseClassAttribute(unsigned long long sexpBeginning);
    
    parameter classAttributeAccess(unsigned long long sexpBeginning);
    
    /* Parse sexps finds s-expressions and passes them to another method, that parses single s-expressions  */
    /* That method is passed to parseSexps() via an std::function object, so I can reuse parseSexps()       */
    void parseSexps(unsigned long long firstSexp, std::function<parameter(Parser*, unsigned long long)> & fun);
    
    void parseDeclarations(); /* Declare functions and global variables */
    void declaration(unsigned long long declBeginning, unsigned long long declEnd);       /* Calls varDeclaration() or funDeclaration(),
                                                                                             exists to increase code readability and 
                                                                                             modularity */
    void varDeclaration(unsigned long long declBeginning, unsigned long long declEnd);    /* Declares a global variable,
                                                                                             called from parseDeclarations() */
    void funDeclaration(unsigned long long declBeginning, unsigned long long declEnd);    /* Declares a function,
                                                                                             called from parseDeclarations() */
    void classDeclaration(unsigned long long declBeginning, unsigned long long declEnd);
    
    void parseFun(unsigned long long funBeginning, unsigned long long funEnd);
    void parseParams(unsigned long long beginning, std::vector<parameter> & params);
    
    /* Outputs includes into the output file */
    void libs();
    /* Outputs typedefs into the output file */
    void typedefs();
    /* Outputs functions into the output file, might change this later */
    // void functions();
    
    void parseDefinitions(); /* Parse definitions */
    void definition(unsigned long long defBeginning, unsigned long long defEnd); /* Same as declaration() */
    
public:
    Parser(std::vector<token> & vectorRef, AbstractSyntaxTree & ast);
    
    void parse();
    
};


#endif /* translator_hpp */