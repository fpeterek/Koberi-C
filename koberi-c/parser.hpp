//
//  parser
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#ifndef parser_hpp
#define parser_hpp

#include <iostream>
#include <vector>
#include <cmath>

#include "syntax.hpp"
#include "token.hpp"
#include "exceptions.hpp"
#include "expressions.hpp"
#include "contains.hpp"
#include "debug_macro_definitions.hpp"
#include "class.hpp"
#include "traversable_ast.hpp"


class Parser {
    
    /* Reference to AST */
    TraversableAbstractSyntaxTree & _ast;
    
    /* Reference to a vector of tokens created by Tokenizer */
    std::vector<token> & _tokens;

    /* Gets type of a literal */
    std::string getType(token & tok);
    
    /* Checks if token at index is a literal */
    bool isLiteral(unsigned long long tokenIndex);
    
    /* Creates a new literal from a token */
    ASTLiteral createLiteral(unsigned long long literalIndex);
    
    /* Parses a single s-expression and emplaces it into the AST */
    void parseSexp(unsigned long long sexpBeginning);
    
    /* Parses a construct, emplaces it into AST and leaves it's scope once it's done */
    void parseConstruct(unsigned long long constructBeginning, unsigned long long constructEnd);
    
    /* Parses calls to functions or operators */
    ASTFunCall parseFunCall(unsigned long long callBeginning, unsigned long long callEnd);
    
    /* Parses member access operator[]                                  */
    /* Accepts a reference so when the function execution finishes,     */
    /* the iterator is set to the end of member access                  */
    /* and member access parameters aren't parsed as separate variables */
    ASTMemberAccess parseMemberAccess(unsigned long long & exprBeginning);
    
    /* Finds index of the end of an s-expression */
    unsigned long long findSexpEnd(unsigned long long sexpBeginning);
    
    /* Parses member functions and class attributes */
    void parseClassMembers(unsigned long long firstSexp, std::string & className);
    
    /* Parses global functions */
    parameter parseVariable(unsigned long long sexpBeginning);
    
    /* Parse sexps finds s-expressions and passes them to parseSexp(), */
    /* a method which parses single s-expressions                      */
    void parseSexps(unsigned long long firstSexp);
    
    /* Declares a global variable, called from parseDeclarations() */
    void globalVarDeclaration(unsigned long long declBeginning, unsigned long long declEnd);
    
    /* Declares a local variable */
    void localVarDeclaration(unsigned long long declBeginning, unsigned long long declEnd);

    /* Parses a class definition and emplaces it into AST */
    void classDefinition(unsigned long long defBeginning, unsigned long long defEnd);
    
    /* Generates a v-table initializer method */
    void generateVtableInitializer(const std::string & className);
    
    /* parseFun() is capable of parsing normal functions, as well as member functions */
    /* If className is an empty string, function will be treated as a normal function */
    /* Otherwise the function will be treated as a member function of class className */
    void parseFun(unsigned long long funBeginning, unsigned long long funEnd, const std::string className = "");
    
    /* Here I am, here I am, the Method Man */
    void parseMethod(const unsigned long long methodBeginning, const std::string & className);
    
    /* Parses function call parameters */
    void parseParams(unsigned long long beginning, std::vector<parameter> & params);
    
    /* Iterates over definitions and parses them */
    void parseDefinitions();
    
    /* Determines whether a definition is a class/function/global variable definition */
    /* and calls method which parses such definition                                  */
    void definition(unsigned long long defBeginning, unsigned long long defEnd);
    
public:
    Parser(std::vector<token> & vectorRef, TraversableAbstractSyntaxTree & ast);
    
    void parse();
    
};


#endif /* parser_hpp */
