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


struct parameter {
    
    std::string type;
    std::string value;
    
};

namespace expr {
    
    /* Can't use an std::map for these functions, because they accept different parameters */
    
    std::string variableDeclaration(std::string & type, std::string & name, std::string & value);
    std::string setNumValue(std::string & var, std::string & value);
    
    std::string print(std::vector<parameter> & params); /* Print string to stdout */
    
    /* + - * / % on ints, + - * / on nums */
    std::string numericOperation(std::string & operation, std::vector<std::string> & nums);
    /* % in C can only be used on ints */
    std::string numMod(std::vector<std::string> & nums);
    
    std::string conversionToNum(parameter & param);
    std::string intToNum(parameter & param);
    std::string strToNum(parameter & param);
    
    std::string conversionToInt(parameter & param);
    std::string numToInt(parameter & param);
    std::string strToInt(parameter & param);

}

#endif /* expressions_hpp */
