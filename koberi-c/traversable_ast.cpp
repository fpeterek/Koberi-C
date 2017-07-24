//
//  traversable_ast.cpp
//  koberi-c
//
//  Created by Filip Peterek on 14/05/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#include "traversable_ast.hpp"

void TraversableAbstractSyntaxTree::enterScope(size_t scopeIndex) {

    ASTNode * newScope = _currentScope->childNodes[scopeIndex];
    
    switch (newScope -> nodeType) {
            

        case NodeType::Scope:
        case NodeType::Function:
        case NodeType::Construct:
            _currentScope = (ASTScope*)newScope;
            break;
            
        default:
            throw not_a_scope();
            break;
            
    }
    
}

void TraversableAbstractSyntaxTree::exitScope() {

    if (_currentScope == &_globalScope) {
        throw exiting_global_scope();
    }
    
    _currentScope = _currentScope->parentScope;
    
}

ASTNode & TraversableAbstractSyntaxTree::getNode(size_t nodeIndex) {

    return *_currentScope->childNodes[nodeIndex];
    
}

ASTNode * TraversableAbstractSyntaxTree::getNodePtr(size_t nodeIndex) {
    
    return _currentScope->childNodes[nodeIndex];
    
}

size_t TraversableAbstractSyntaxTree::getNodeCount() {
    
    return _currentScope->childNodes.size();
    
}

ASTScope & TraversableAbstractSyntaxTree::getCurrentScope() {

    return *_currentScope;
    
}

ASTScope * TraversableAbstractSyntaxTree::getCurrentScopePtr() {

    return _currentScope;
    
}


ASTScope & TraversableAbstractSyntaxTree::getGlobalScope() {
    
    return _globalScope;
    
}

ASTScope * TraversableAbstractSyntaxTree::getGlobalScopePtr() {
    
    return &_globalScope;
    
}

std::string TraversableAbstractSyntaxTree::getVarTypeRecursive(const std::string & varName, ASTScope * scope) {
    
    try {
        
        return scope->vars.at(varName);
    
    } catch (const std::out_of_range & e ) {
        
        if (scope == &_globalScope) {
            
            throw undefined_variable(varName);
        
        }
        
        return getVarTypeRecursive(varName, scope->parentScope);
        
    }
    
}

std::string TraversableAbstractSyntaxTree::getVarType(const std::string & varName) {
    
    return getVarTypeRecursive(varName, _currentScope);
    
}

std::string TraversableAbstractSyntaxTree::getFunctionReturnType(const std::string & funName) {
    
    return _functions.at(funName);
    
}

const std::unordered_map<std::string, _class> & TraversableAbstractSyntaxTree::getClasses() {
    return _classes;
}

const std::vector<std::string> & TraversableAbstractSyntaxTree::getClassOrder() {
    return _classOrder;
}

bool TraversableAbstractSyntaxTree::isDataType(const std::string & param) {
    
    return contains(_dataTypes, param);
    
}

