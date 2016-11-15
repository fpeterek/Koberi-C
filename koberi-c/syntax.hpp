//
//  syntax.hpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#ifndef syntax_hpp
#define syntax_hpp

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <array>

namespace syntax {
    
    const std::array<char, 13> operatorChar = {
        
        '=', '+', '-', '*', '/', '%', '<', '>', '!', '&', '|', '~', '^'
        
    };
    
    const std::array<std::string, 31> operators = {
        
        "=",
        "+", "+=",
        "-", "-=",
        "*", "*=",
        "/", "/=",
        "%", "%=",
        "++", "--",
        
        // Comparison operators
        
        "<", ">",
        "<=", ">=",
        "==", "!=",
        
        // Bitwise operators
        
        "&", "&=",
        "|", "|=",
        "<<", "<<=",
        ">>", ">>",
        "~", "~=",
        "^", "^="
        
        /*
         
         // Logic operators
         
         "and", "or",
         
         // Negation operator
         
         "not",
         
         */
        
    };
    
    bool isNum(char param);
    bool isValidIdChar(char param); /* Returns true if character is a valid character for a var/func name */
    bool isWhiteSpace(char param);
    bool isOperator(std::string & param); /* Returns true if string is a koberi-c operator */
    bool isOperatorChar(char param);
    
    
}


#endif /* syntax_hpp */
