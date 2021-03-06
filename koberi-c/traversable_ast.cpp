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
    
    if (scope->vars.count(varName)) {
        return scope->vars.at(varName);
    }
    if (scope == &_globalScope) {
        throw undefined_variable(varName);
    }
    return getVarTypeRecursive(varName, scope->parentScope);
    
}

std::string TraversableAbstractSyntaxTree::getVarType(const std::string & varName) {
    
    return getVarTypeRecursive(varName, _currentScope);
    
}

std::string TraversableAbstractSyntaxTree::getVarType(const std::string & varName, ASTScope * scope) {
    
    return getVarTypeRecursive(varName, scope);
    
}

std::string TraversableAbstractSyntaxTree::getFunctionReturnType(const std::string & funName) {
    
    try {
        
        return _functions.at(funName);
    
    } catch (const std::out_of_range & e) {
    
        throw undeclared_function_call(funName);
    
    }
    
}

const _class & TraversableAbstractSyntaxTree::getClass(const std::string & className) {
    
    try {
        
        return _classes.at(className);
    
    } catch (const std::out_of_range & e) {
        
        throw undefined_class(className);
        
    }
    
}

const std::unordered_map<std::string, _class> & TraversableAbstractSyntaxTree::getClasses() {
    return _classes;
}

const std::vector<std::string> & TraversableAbstractSyntaxTree::getClassOrder() {
    return _classOrder;
}

bool TraversableAbstractSyntaxTree::isDataType(const std::string & param) {
    
    std::string type = param;
    
    /* If type is a pointer, check if it's a pointer to an existing type */
    if (type.back() == '*') {
        type.pop_back();
    }
    
    return contains(_dataTypes, type);
    
}

bool TraversableAbstractSyntaxTree::isClass(const std::string & param) {
    
    /* Check if param isn't a primitive type. If it isn't, check if it is an existing type. */
    return (not contains(_primitiveTypes, param)) and isDataType(param);
    
}

bool TraversableAbstractSyntaxTree::hasSuperclass(const std::string & className, const std::string & superClass) {
    
    if (not _classes.count(className)) {
        throw not_a_class(className);
    }
    
    const _class & c = _classes.at(className);
    
    if (c.superClass == "") {
        return false;
    }
    
    if (c.superClass == superClass) {
        return true;
    }
    
    return hasSuperclass(c.superClass, superClass);
    
}

bool TraversableAbstractSyntaxTree::hasMethod(const std::string & methodName, const std::string & className) {
    
    if (not _classes.count(className)) {
        throw not_a_class(className);
    }
    
    _class & c = _classes.at(className);
    
    if (c.methods.count(methodName)) {
        return true;
    }
    
    if (c.superClass == "") {
        return false;
    }
    
    return hasMethod(methodName, c.superClass);
    
}

bool TraversableAbstractSyntaxTree::hasDestructor(const std::string & className) {
    
    std::string destructor =  NameMangler::mangleName("destruct", std::vector<parameter>());
    
    return hasMethod(destructor, className);
    
}

method TraversableAbstractSyntaxTree::getMethodReturnType(const std::string & methodName, const std::string & className) {
    
    if (not _classes.count(className)) {
        throw not_a_class(className);
    }
    
    _class & c = _classes.at(className);
    
    if (c.methods.count(methodName)) {
        
        method meth;
        
        meth.type = c.methods.at(methodName);
        meth.className = className;
        
        return meth;
        
    }
    
    if (c.superClass == "") {
        throw undeclared_function_call(methodName);
    }
    
    return getMethodReturnType(methodName, c.superClass);
    
}
