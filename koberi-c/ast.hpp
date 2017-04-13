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
    /* Keeps track of functions with mangled names, whereas globalScope holds names before mangling */
    std::unordered_map<std::string, std::string> _functions;
    
    /* Defines the global scope, since the global scope has no parent, parentScope points to 0 */
    ASTScope _globalScope;
    
public:
    AbstractSyntaxTree();
    
    void emplaceFunction(const std::string & functionName,
                         const std::string & returnType,
                         const std::vector<parameter> & params);
    
    std::string getVariableType(const std::string & varName);
    
    void leaveScope();
    void newScope();
    
    
};

#endif /* ast_hpp */
