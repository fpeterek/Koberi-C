//
//  parameter.cpp
//  koberi-c
//
//  Created by Filip Peterek on 02/04/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#include "parameter.hpp"

parameter::parameter() : name(value) {
    
    value = "";
    type = "";
    
}

parameter::parameter(const std::string & newValue) : name(value) {
    
    value = newValue;
    
}

parameter::parameter(const std::string & newValue, const std::string & newType) : name(value) {
    
    value = newValue;
    type = newType;
    
}

parameter::parameter(const parameter & param) : name(this->value) {
    
    value = param.value;
    type = param.type;
    
}

parameter & parameter::operator= (const parameter & param) {
    
    value = param.value;
    type = param.type;
    
    return *this;
    
}
