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

struct parameter {
    
    std::string type;
    std::string value;
    
    parameter();
    parameter(const std::string & value);
    parameter(const std::string & value, const std::string & type);
    
};

namespace expr {
    
    const std::array<std::string, 4> native_types  = {"int", "num", "str", "void"};
    const std::array<std::string, 3> parameterless_operators = { "return", "break", "continue" };
    const std::array<std::string, 6> unary_operators = { "return", "size_of", "inc", "dec", "not", "compl" };
    const std::array<std::string, 19> binary_operators = { "+", "-", "*", "/", "mod", "set", "equals", "not_eq", ">",
    "<", ">=", "<=", "and", "or", "bit_and", "bit_or", "xor", "lshift", "rshift"};
    
    const std::unordered_map<std::string /* Kobeři-C function */, std::string /* C operator */ > unary_operators_map = {
    
        {"return", "return"},
        {"size_of", "sizeof"},
        {"inc", "++"},
        {"dec", "--"},
        {"not", "!"},
        {"compl", "~"},
    
    };
    
    const std::unordered_map<std::string, std::string> binary_operators_map = {
        
        {"+", "+"}, {"-", "-"}, {"*", "*"}, {"/", "/"}, {"%", "%"}, {"set", "="}, {"equals", "=="}, {"not_eq", "!="},
        {">", ">"}, {"<", "<"}, {">=", ">="}, {"<=", "<="}, {"and", "&&"}, {"or", "||"}, {"bit_and", "&"},
        {"bit_or", "|"}, {"xor", "^"}, {"lshift", "<<"}, {"rshift", ">>"}, {"mod", "%"}
        
    };
    
    /* Can't use an std::map for these functions, because they accept different parameters */
    
    parameter variableDeclaration(std::string & type, std::string & name, std::string & value);
    parameter variableDeclaration(std::string & type, std::string & name);
    parameter setNumValue(std::string & var, std::string & value);
    
    parameter print(std::vector<parameter> & params); /* Print string to stdout */
    parameter inlineC(std::vector<parameter> & params);
    
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
