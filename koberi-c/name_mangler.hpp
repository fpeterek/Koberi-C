//
//  name_mangler.hpp
//  koberi-c
//
//  Created by Filip Peterek on 23/07/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#ifndef name_mangler_hpp
#define name_mangler_hpp

#include <stdio.h>
#include <vector>
#include <string>

#include "parameter.hpp"

class NameMangler {
    
    const static std::string prefix;
    
public:
    
    /* Mangles function names by parameter types */
    static std::string mangleName(const std::string & name, const std::vector<parameter> & params);
    static std::string mangleName(const std::string & name, const std::vector<std::string> & paramTypes);
    
    /* Premangles method names by class name, but not by parameters */
    static std::string premangleMethodName(const std::string & name, const std::string & className);
    
};


#endif /* name_mangler_hpp */
