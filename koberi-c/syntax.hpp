//
//  syntax.hpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#ifndef syntax_hpp
#define syntax_hpp

#include <string>
#include <unordered_map>
#include <array>

/* Namespace which holds information regarding basic Kobeři-C syntax */

namespace syntax {
    
    /* File extension for Kobeři-C files */
    const std::string fileExtension = ".koberice";
    
    /* Holds special operator characters */
    const std::array<char, 9> operatorChar = {
        
        '=', '+', '-', '*', '/', '%', '<', '>', '&'
        
    };
    
    /* Holds names of C typedefs for Kobeři-C data types */
    const std::string floatType = "flt_type";
    const std::string intType = "int_type";
    const std::string uintType = "uint_type";
    
    /* Vtable initializer method name */
    const std::string vtableInit = "_vtable_init";
    
    /* Object memory allocator/vtable initializer function */
    const std::string memoryAlloc = "_mem_allocator";
    
    /* Character used to declare pointers */
    const char pointerChar = '*';
    
    /* Holds operators made up of special operator characters */
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
    
    /* Returns true if param is a character between '0' and '9' */
    bool isNum(char param);
    /* Returns true if character is a valid character for a var/func name */
    bool isValidIdChar(char param);
    bool isWhiteSpace(char param);
    /* Returns true if string is a koberi-c operator comprised of operator characters */
    bool isOperator(std::string & param);
    bool isOperatorChar(char param);
    
    /* Returns pointer type for type (type + pointerChar) */
    std::string pointerForType(const std::string & type);
    /* Returns true if last character is pointerChar */
    bool isPointerType(const std::string & type);
    
}


#endif /* syntax_hpp */
