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
    
    std::vector<token> & _tokens;

    /* Gets type of a literal */
    std::string getType(token & tok);
    
    /* Checks if token at index is a literal */
    bool isLiteral(unsigned long long tokenIndex);
    
    /* Creates a new literal from a token                                             */
    /* The code appears multiple times in the parser, so I'm turning it into a method */
    ASTLiteral createLiteral(unsigned long long literalIndex);
    
    /* parseSexp() parses a single s-expression and emplaces it into the AST */
    void parseSexp(unsigned long long sexpBeginning);
    
    void parseConstruct(unsigned long long constructBeginning, unsigned long long constructEnd);
    ASTFunCall parseFunCall(unsigned long long callBeginning, unsigned long long callEnd);
    
    unsigned long long findSexpEnd(unsigned long long sexpBeginning);
    
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
    
    /* Declares a local variable */
    void localVarDeclaration(unsigned long long declBeginning, unsigned long long declEnd);

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
