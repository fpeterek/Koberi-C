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

namespace expr {
    
    /* Can't use an std::map for these functions, because they accept different parameters */
    
    std::string variableDeclaration(std::string & type, std::string & name, std::string & value);
    std::string setNumValue(std::string & var, std::string & value);
    
    std::string print(std::vector<std::string> & str); /* Print string to stdout */
    
    /* + - * / % on ints, + - * / on nums */
    std::string numericOperation(std::string & operation, std::vector<std::string> & nums);
    /* % in C can only be used on ints */
    std::string numMod(std::vector<std::string> & nums);

}

#endif /* expressions_hpp */
