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
#include "debug_macro_definitions.hpp"
#include "class.hpp"
#include "traversable_ast.hpp"


class Parser {
    
    TraversableAbstractSyntaxTree & _ast;
    
    /* --------------------------------------------------------------------------------------------------------- */
    /*                                                                                                           */
    /* std::unordered_map<std::string, _class> _classes; // Keeping this here as a reminder of my stupid mistake */
    /*                                                                                                           */
    /* --------------------------------------------------------------------------------------------------------- */
    
    std::vector<token> & _tokens;

    /* Gets type of a variable or literal */
    std::string getType(token & tok);
    
    /* parseSexp() parses a single s-expression and emplaces it into the AST */
    void parseSexp(unsigned long long sexpBeginning);
    
    /* Similar to parseSexps(), but adjusted to work properly on classes */
    std::vector<parameter> parseClassMembers(unsigned long long firstSexp, std::string & className);
    
    /* Similar to parseSexp(), but only parses class attributes and global variables */
    parameter parseVariable(unsigned long long sexpBeginning);
    
    /* Parse sexps finds s-expressions and passes them to another method, that parses single s-expressions  */
    /* That method is passed to parseSexps() via an std::function object, so I can reuse parseSexps()       */
    void parseSexps(unsigned long long firstSexp);
    
    /* Calls varDeclaration() or funDeclaration(), */
    /* exists to increase code readability and     */
    /* modularity                                  */
    void declaration(unsigned long long declBeginning, unsigned long long declEnd);
    
    void varDeclaration(unsigned long long declBeginning, unsigned long long declEnd);    /* Declares a global variable,
                                                                                             called from parseDeclarations() */

    void classDefinition(unsigned long long defBeginning, unsigned long long defEnd);
    
    void parseFun(unsigned long long funBeginning, unsigned long long funEnd);
    void parseParams(unsigned long long beginning, std::vector<parameter> & params);
    
    void parseDefinitions(); /* Parse definitions */
    void definition(unsigned long long defBeginning, unsigned long long defEnd); /* Same as declaration() */
    
public:
    Parser(std::vector<token> & vectorRef, TraversableAbstractSyntaxTree & ast);
    
    void parse();
    
};


#endif /* translator_hpp */
