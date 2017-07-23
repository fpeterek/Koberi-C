//
//  translator.hpp
//  koberi-c
//
//  Created by Filip Peterek on 22/07/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#ifndef translator_hpp
#define translator_hpp

#include "traversable_ast.hpp"

class Translator {
    
    TraversableAbstractSyntaxTree & _ast;
    
public:
    
    Translator(TraversableAbstractSyntaxTree & ast);
    
    void translate();
    
};

#endif /* translator_hpp */
