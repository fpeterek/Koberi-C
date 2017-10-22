//
//  syntax.cpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#include "syntax.hpp"


bool syntax::isNum(char param) {
    
    return ( param >= '0' and param <= '9' );
}

bool syntax::isValidIdChar(char param) {
    
    return ( (param >= 'a' and param <= 'z') or (param >= 'A' and param <= 'Z') or param == '_');
    
}

bool syntax::isWhiteSpace(char param) {
    
    return isspace(param);
    
}

bool syntax::isOperator(std::string & param) {
    
    for (auto & i : syntax::operators) {
        
        if (param == i) { return true; }
        
    }
    
    return false;
    
}

bool syntax::isOperatorChar(char param) {
    
    for (auto i : syntax::operatorChar) {
        
        if (param == i) { return true; }
        
    }
    
    return false;
    
}
