//
//  token.hpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#ifndef token_hpp
#define token_hpp

#include <string>

enum class tokType {
    
    openingPar,     //  Opening parenthesis '('
    closingPar,     //  Closing parenthesis ')'
    openingBra,     //  Opening bracket '['         // It has nothing to do with bras
    closingBra,     //  Closing bracket ']'
    numLit,         //  Floating point number literal
    intLit,         //  Integer literal
    charLit,        //  Character literal, eg. 'K'
    strLit,         //  String literal, eg. "Kobeřice"
    id,             //  An identifier, eg. add
    op,             //  An operator, eg. +
    none            //  Nothing
    
};

/* Single lexed token */

struct token {
    
    tokType type;
    std::string value;
    
    token(tokType t, std::string val);
    token();
    
    bool operator== (tokType comparedType);
    bool operator!= (tokType comparedType);
    
};


#endif /* token_hpp */
