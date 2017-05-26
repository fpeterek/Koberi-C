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
    
    /* Use value for literals, name for variable names */
    /* They both refer to the same object */
    
    /* Value of a literal */
    std::string value;
    /* Reference to parameter::value, used for variable names */
    std::string & name;
    
    parameter();
    parameter(const std::string & value);
    parameter(const std::string & value, const std::string & type);
    parameter(const parameter & param);
    
    parameter & operator= (const parameter & param);
    
};

#endif /* parameter_hpp */
