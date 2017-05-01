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
    
    _dataTypes = { "num", "int" };
    
}

std::string AbstractSyntaxTree::mangleName(const std::string & name, const std::vector<parameter> & parameters) {

    std::string mangledName = "_" + name;
    
    for (auto & param : parameters) {
        mangledName.append("_" + param.type);
    }
    
    return mangledName;

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
    
    _functions.emplace(mangleName(functionName, params), returnType);
    
}

void AbstractSyntaxTree::emplaceConstruct(const std::string & construct,
                                          const ASTFunCall & condition) {

    ASTConstruct * constructPtr = new ASTConstruct(_currentScope, construct, condition);
    
    _currentScope -> childNodes.emplace_back(constructPtr);
    
    _currentScope = (ASTScope*)_currentScope->childNodes.back();

}

void AbstractSyntaxTree::emplaceFunCall(const std::string & name,
                                        const std::vector<parameter> & params) {
    
    ASTFunCall * funcall = new ASTFunCall(_currentScope, name, params);
    
    _currentScope -> childNodes.emplace_back(funcall);

}

void AbstractSyntaxTree::emplaceDeclaration(const std::string & type,
                                            const std::string & name,
                                            const std::string & value) {
    
    ASTDeclaration * declaration = new ASTDeclaration(_currentScope, type, name, value);
    _currentScope -> childNodes.emplace_back(declaration);
    
}

void AbstractSyntaxTree::leaveScope() {

    if (_currentScope -> parentScope == nullptr) {
        throw exiting_global_scope();
    }
    
    _currentScope = _currentScope -> parentScope;

}
