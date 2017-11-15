//
//  expressions.hpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#ifndef expressions_hpp
#define expressions_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <array>

#include "exceptions.hpp"
#include "print.hpp"
#include "parameter.hpp"
#include "contains.hpp"
#include "syntax.hpp"
#include "aast_node.hpp"


namespace expr {
    
    const std::array<std::string, 4> native_types  = {"int", "num", "char", "void"};
    const std::array<std::string, 3> parameterless_operators = { "return", "break", "continue" };
    const std::array<std::string, 10> unary_operators = { "return", "size_of", "inc", "dec", "not", "compl", "-", "&",
    "new", "delete" };
    const std::array<std::string, 19> binary_operators = { "+", "-", "*", "/", "mod", "set", "equals", "not_eq", ">",
    "<", ">=", "<=", "and", "or", "bit_and", "bit_or", "xor", "lshift", "rshift"};
    
    const std::array<std::string, 30> operators = {
        "return", "break", "continue",
        "size_of", "inc", "dec", "not", "compl",
        "+", "-", "*", "/", "mod", "set", "equals", "not_eq", ">",
        "<", ">=", "<=", "and", "or", "bit_and", "bit_or", "xor", "lshift", "rshift",
        "&", "new", "delete"
    };
    
    const std::vector<std::string> keywords = {
        /* Types */
        "int", "char", "void", "unsigned", "long", "short", "num", "auto",
        "const", "static", "double", "float", "register", "restrict", "signed",
        "volatile", "var",
        /* Constructs */
        "if", "else", "elif", "while", "for", "do", "switch", "case", "default"
        /* Operators */
        "return", "break", "continue", "size_of", "sizeof", "inc", "dec", "equals",
        "not_eq", "and", "or", "not", "compl", "set", "lshift", "rshift", "xor", "bit_or",
        "bit_and", "goto"
        /* Other */
        "typedef", "struct", "extern", "inline", "union",
        /* Kobeři-C keywords */
        "_c", "new", "delete"
    };
    
    const std::array<std::string, 4> constructs = { "if", "elsif", "else", "while" };
    
    const std::unordered_map<std::string /* Kobeři-C function */, std::string /* C operator */ > unary_operators_map = {
    
        {"return", "return"},
        {"size_of", "sizeof"},
        {"inc", "++"},
        {"dec", "--"},
        {"not", "!"},
        {"compl", "~"},
        {"-", "-"},
        {"&", "&"}
    
    };
    
    const std::unordered_map<std::string, std::string> binary_operators_map = {
        
        {"+", "+"}, {"-", "-"}, {"*", "*"}, {"/", "/"}, {"%", "%"}, {"set", "="}, {"equals", "=="}, {"not_eq", "!="},
        {">", ">"}, {"<", "<"}, {">=", ">="}, {"<=", "<="}, {"and", "&&"}, {"or", "||"}, {"bit_and", "&"},
        {"bit_or", "|"}, {"xor", "^"}, {"lshift", "<<"}, {"rshift", ">>"}, {"mod", "%"}
        
    };
    
    const std::array<std::string, 5> numerical_types = { "char", "uchar", "int", "uint", "num" };
    
    bool isConstruct(const std::string & construct);
    
    bool isOperator(const std::string & op);
    
    bool isParameterlessOperator(const std::string & op);
    bool isUnaryOperator(const std::string & op);
    bool isBinaryOperator(const std::string & op);
    
    bool isNumericalType(const std::string & type);
    
    
    AASTOperator * binaryOperator(std::vector<AASTNode *> & params, const std::string & op);
    AASTOperator * mod(std::vector<AASTNode *> & params);
    AASTOperator * comparison(std::vector<AASTNode *> & params, const std::string & op);
    AASTOperator * set(const AASTNode * lvalue, const AASTNode * rvalue);
    
    AASTOperator * unaryOperator(AASTNode * param, const std::string & op);
    AASTOperator * parameterless_operator(const std::string & op);
    
    AASTOperator * reference(const AASTNode * param);
    
    AASTNode * dereferencePtr(AASTNode * param);

}

#endif /* expressions_hpp */
