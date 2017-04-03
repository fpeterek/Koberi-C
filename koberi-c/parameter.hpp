//
//  parameter.hpp
//  koberi-c
//
//  Created by Filip Peterek on 02/04/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#ifndef parameter_hpp
#define parameter_hpp

#include <stdio.h>
#include <string>

struct parameter {
    
    std::string type;
    /* Either variable name or a literal */
    std::string value;
    
    parameter();
    parameter(const std::string & value);
    parameter(const std::string & value, const std::string & type);
    
};

#endif /* parameter_hpp */
