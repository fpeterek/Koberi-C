//
//  expressions.hpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#ifndef expressions_hpp
#define expressions_hpp

#include <vector>
#include <unordered_map>
#include <array>

#include "exceptions.hpp"
#include "parameter.hpp"
#include "contains.hpp"
#include "syntax.hpp"
#include "aast_node.hpp"

/* Used to handle certain expressions, especially operators */
/* Also provides certain functionality regarding data types */

namespace expr {
    
    /* An array of primitive Kobeři-C types */
    const std::array<std::string, 6> native_types  = { "int", "uint", "num", "char", "uchar", "void" };
    
    /* An array of operators which take no parameters */
    const std::array<std::string, 3> parameterless_operators = { "return", "break", "continue" };
    
    /* An array of operators which take one parameter */
    const std::array<std::string, 10> unary_operators = { "return", "size_of", "inc", "dec", "not", "compl", "-", "&",
    "new", "delete" };
    
    /* An array of operators which take two parameters */
    /* Most of these operators are binary in C but variadic in Kobeři-C */
    const std::array<std::string, 19> binary_operators = { "+", "-", "*", "/", "mod", "set", "equals", "not_eq", ">",
    "<", ">=", "<=", "and", "or", "bit_and", "bit_or", "xor", "lshift", "rshift"};
    
    /* An array of all operators combined */
    const std::array<std::string, 30> operators = {
        
        "return", "break", "continue",
        "size_of", "inc", "dec", "not", "compl",
        "+", "-", "*", "/", "mod", "set", "equals", "not_eq", ">",
        "<", ">=", "<=", "and", "or", "bit_and", "bit_or", "xor", "lshift", "rshift",
        "&", "new", "delete"
        
    };
    
    /* C and Kobeři-C keywords, which can't be used as identifiers */
    const std::vector<std::string> keywords = {
        /* Types */
        "int", "char", "void", "unsigned", "long", "short", "num", "auto",
        "const", "static", "double", "float", "register", "restrict", "signed",
        "volatile", "var",
        /* Constructs */
        "if", "else", "elif", "while", "dowhile", "for", "do", "switch", "case", "default"
        /* Operators */
        "return", "break", "continue", "size_of", "sizeof", "inc", "dec", "equals",
        "not_eq", "and", "or", "not", "compl", "set", "lshift", "rshift", "xor", "bit_or",
        "bit_and", "goto"
        /* Other */
        "typedef", "struct", "extern", "inline", "union",
        /* Kobeři-C keywords */
        "_c", "new", "delete"
    };
    
    /* Kobeři-C control flow constructs */
    const std::array<std::string, 5> constructs = { "if", "elif", "else", "while", "dowhile" };
    
    /* Map which holds Kobeři-C unary operators and their respective operators in C */
    const std::unordered_map<std::string /* Kobeři-C operator */, std::string /* C operator */ > unary_operators_map = {
    
        {"return", "return"},
        {"size_of", "sizeof"},
        {"inc", "++"},
        {"dec", "--"},
        {"not", "!"},
        {"compl", "~"},
        {"-", "-"},
        {"&", "&"}
    
    };
    
    /* Map which holds Kobeři-C binary operators and their respective operators in C */
    const std::unordered_map<std::string, std::string> binary_operators_map = {
        
        {"+", "+"}, {"-", "-"}, {"*", "*"}, {"/", "/"}, {"%", "%"}, {"set", "="}, {"equals", "=="}, {"not_eq", "!="},
        {">", ">"}, {"<", "<"}, {">=", ">="}, {"<=", "<="}, {"and", "&&"}, {"or", "||"}, {"bit_and", "&"},
        {"bit_or", "|"}, {"xor", "^"}, {"lshift", "<<"}, {"rshift", ">>"}, {"mod", "%"}
        
    };
    
    /* Array of numerical types, which can easily be casted from one to another */
    const std::array<std::string, 5> numerical_types = { "char", "uchar", "int", "uint", "num" };
    
    /* Checks if parameter is a control flow construct */
    bool isConstruct(const std::string & construct);
    
    /* Checks if parameter is a Kobeři-C operator */
    bool isOperator(const std::string & op);
    
    /* Checks if parameter is a valid Kobeři-C parameterless operator */
    bool isParameterlessOperator(const std::string & op);
    /* Checks if parameter is a valid Kobeři-C unary operator */
    bool isUnaryOperator(const std::string & op);
    /* Checks if parameter is a valid Kobeři-C binary operator */
    bool isBinaryOperator(const std::string & op);
    
    /* Checks if type is a numerical type */
    bool isNumericalType(const std::string & type);
    
    /* Sets verbose mode to true/false */
    void setVerbose(bool value);
    bool isVerbose();
    
    /* Analyzes binary operators */
    AASTOperator * binaryOperator(std::vector<AASTNode *> & params, const std::string & op);
    /* Analyzes mod operator */
    AASTOperator * mod(std::vector<AASTNode *> & params);
    /* Analyzes comparisons */
    AASTOperator * comparison(std::vector<AASTNode *> & params, const std::string & op);
    /* Analyzes set (=) operator */
    AASTOperator * set(const AASTNode * lvalue, const AASTNode * rvalue);
    
    /* Analyzes unary operators */
    AASTOperator * unaryOperator(AASTNode * param, const std::string & op);
    /* Analyzes parameterless operators */
    AASTOperator * parameterless_operator(const std::string & op);
    
    /* Gets reference to param using & C operator */
    AASTOperator * reference(const AASTNode * param);
    
    /* Dereferences pointer using * C operator */
    AASTNode * dereferencePtr(AASTNode * param);

}

#endif /* expressions_hpp */
