//
//  parameter.cpp
//  koberi-c
//
//  Created by Filip Peterek on 02/04/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#include "parameter.hpp"

parameter::parameter() {
    
    value = "";
    type = "";
    
}

parameter::parameter(const std::string & newValue) {
    
    value = newValue;
    
}

parameter::parameter(const std::string & newValue, const std::string & newType) {
    
    value = newValue;
    type = newType;
    
}
