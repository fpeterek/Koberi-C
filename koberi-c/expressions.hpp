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
        {"&", "&"},
        {"delete", "free"}
    
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
    
    /* Can't use an std::map for these functions, because they accept different parameters */
    
    parameter variableDeclaration(std::string & type, std::string & name, std::string & value);
    parameter variableDeclaration(std::string & type, std::string & name);
    parameter setNumValue(std::string & var, std::string & value);
    
    std::vector<std::string> print(std::vector<parameter> & params); /* Print string to stdout */
    std::vector<std::string> inlineC(std::vector<parameter> & params);
    
    /* + - * / mod on ints, + - * / on nums */
    parameter binaryOperator(std::vector<parameter> & params, std::string & op);
    /* % in C can only be used on ints */
    parameter numMod(std::vector<parameter> & nums);
    
    parameter comparison(std::string & op, std::vector<parameter> & params);
    
    
    parameter conversionToNum(parameter & param);
    parameter intToNum(parameter & param);
    parameter strToNum(parameter & param);
    parameter voidToNum(parameter & param);
    
    parameter conversionToInt(parameter & param);
    parameter numToInt(parameter & param);
    parameter strToInt(parameter & param);
    parameter voidToInt(parameter & param);
    
    parameter unaryOperator(parameter & param, std::string & op);
    parameter parameterless_operator(std::string & op);

}

#endif /* expressions_hpp */
