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
    
    static std::string mangleName(const std::string & name, const std::vector<parameter> & params);
    static std::string mangleName(const std::string & name, const std::vector<std::string> & paramTypes);
    
};


#endif /* name_mangler_hpp */
