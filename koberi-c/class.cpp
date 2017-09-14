//
//  class.cpp
//  koberi-c
//
//  Created by Filip Peterek on 02/04/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#include "class.hpp"

std::string _class::getVarType(const std::string & name) const {
    
    for (const parameter & i : attributes) {
        
        if (i.name == name) {
            return i.type;
        }
        
    }
    
    throw no_such_member(name, className);
    
}

bool _class::hasVar(const std::string & name) const {
    
    for (const parameter & i : attributes) {
        
        if (i.name == name) {
            return true;
        }
        
    }
    
    return false;
    
}
