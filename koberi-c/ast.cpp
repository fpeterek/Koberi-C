//
//  ast.cpp
//  koberi-c
//
//  Created by Filip Peterek on 02/04/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#include "ast.hpp"

AbstractSyntaxTree::AbstractSyntaxTree() : _globalScope(nullptr) {
    
    _currentScope = &_globalScope;
    
}

void AbstractSyntaxTree::emplaceFunction(const std::string & functionName,
                                         const std::string & returnType,
                                         const std::vector<parameter> & params) {
    
    /* Functions must be in the global scope */
    _currentScope = &_globalScope;
    ASTFunction * function = new ASTFunction(&_globalScope, functionName, returnType, params);
    
    _globalScope.childNodes.emplace_back(function);
    /* Somehow I don't believe this is gonna work */
    /* Edit: It should work now that I fixed it   */
    _currentScope = (ASTScope*)_globalScope.childNodes.back();
    
}

void AbstractSyntaxTree::emplaceConstruct(const std::string & construct,
                                          const ASTFunCall & condition) {

    ASTConstruct * constructPtr = new ASTConstruct(_currentScope, construct, condition);
    
    _currentScope -> childNodes.emplace_back(constructPtr);

}
