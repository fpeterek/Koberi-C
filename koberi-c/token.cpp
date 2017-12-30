//
//  token.cpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#include "token.hpp"


token::token(tokType t, const std::string & val) {
    
    type = t; value = val;
    
}

token::token() {
    
    type = tokType::none; value = "";
    
}

bool token::operator== (tokType comparedType) {

    return type == comparedType;
    
}

bool token::operator!= (tokType comparedType) {
    
    return type != comparedType;
    
}
