//
//  class.hpp
//  koberi-c
//
//  Created by Filip Peterek on 02/04/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#ifndef class_hpp
#define class_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "parameter.hpp"
#include "exceptions.hpp"

struct _class {
    
    std::string className;
    /* Implementing this as a vector because I want the variables sorted */
    std::vector<parameter> vars;
    std::unordered_map<std::string, std::string> methods;
    
    std::string getVarType(const std::string & name) const;
    bool hasVar(const std::string & name) const;
    
};

#endif /* class_hpp */
