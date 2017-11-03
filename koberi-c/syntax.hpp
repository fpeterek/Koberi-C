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
    
    const std::string fileExtension = ".koberice";
    
    const std::array<char, 9> operatorChar = {
        
        '=', '+', '-', '*', '/', '%', '<', '>', '&'
        
    };
    
    const std::string floatType = "num_type";
    const std::string intType = "int_type";
    const std::string uintType = "uint_type";
    const char pointerChar = '*';
    
    const std::array<std::string, 10> operators = {
        
        "+", "-",
        "*", "/",
        "%",
        
        // Comparison operators
        
        "<", ">",
        "<=", ">=",
        
        // Memory address access operator
        
        "&"
        
    };
    
    bool isNum(char param);
    bool isValidIdChar(char param); /* Returns true if character is a valid character for a var/func name */
    bool isWhiteSpace(char param);
    bool isOperator(std::string & param); /* Returns true if string is a koberi-c operator */
    bool isOperatorChar(char param);
    
    std::string pointerForType(const std::string & type);
    
}


#endif /* syntax_hpp */
