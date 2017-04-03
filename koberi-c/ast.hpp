//
//  ast.hpp
//  koberi-c
//
//  Created by Filip Peterek on 02/04/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#ifndef ast_hpp
#define ast_hpp

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <functional>

#include "ast_node.hpp"
#include "exceptions.hpp"

class AbstractSyntaxTree {
    
    /* Format: Name - Data Type */
    /* Might not use this since globalScope keeps track of functions too */
    /* Functions are found faster in a map than in an array              */
    /* But doing this would result in storing duplicate data             */
    std::unordered_map<std::string, std::string> _functions;  /* Keeps track of functions */
    
    /* Defines the global scope, since the global scope has no parent, parentScope points to 0 */
    ASTScope _globalScope;
    
public:
    AbstractSyntaxTree();
    
    
};

#endif /* ast_hpp */
