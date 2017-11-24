//
//  parameter.hpp
//  koberi-c
//
//  Created by Filip Peterek on 02/04/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#ifndef parameter_hpp
#define parameter_hpp

#include <string>

/* Holds two values - value and data type                            */
/* Kinda like a tuple, except the values have a name                 */
/* Used to store variable names/literal values etc. and their types  */
/* It got it's name because it was originally used to store function */
/* parameters, though it's probably used in other situations, too    */

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
