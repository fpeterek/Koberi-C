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

void AbstractSyntaxTree::checkType(const std::string & type) {
    
    if (_dataTypes >> type) {
        return;
    }
    throw bad_type("Error: Unknown type: " + type);
    
}

void AbstractSyntaxTree::emplaceFunction(const std::string & functionName,
                                         const std::string & returnType,
                                         const std::vector<parameter> & params) {
    
    /* Functions must be defined in the global scope in C */
    if (_currentScope != &_globalScope) {
        throw wrong_scope("Functions can only be defined in the global scope. ");
    }
    
    /* Check if all types exist */
    for (const auto & param : params) {
        checkType(param.type);
    }
    checkType(returnType);
    
    ASTFunction * function = new ASTFunction(&_globalScope, functionName, returnType, params);
    
    _globalScope.childNodes.emplace_back(function);
    
    _currentScope = (ASTScope*)_globalScope.childNodes.back();
    
    for (const auto & param : params) {
        
        /* Check if parameter doesn't already exist */
        if (_currentScope->vars[param.name] != "") {
            throw redefinition_of_variable(param.name);
        }
        
        _currentScope->vars.emplace(param.name, param.type);
        
    }
    
    _functions.emplace(NameMangler::mangleName(functionName, params), returnType);
    
}

void AbstractSyntaxTree::emplaceConstruct(const std::string & construct,
                                          const ASTFunCall & condition) {

    ASTConstruct * constructPtr = new ASTConstruct(_currentScope, construct, condition);
    
    _currentScope -> childNodes.emplace_back(constructPtr);
    
    _currentScope = (ASTScope*)_currentScope->childNodes.back();

}

void AbstractSyntaxTree::emplaceFunCall(const std::string & name,
                                        const std::vector<ASTNode *> & params) {
    
    ASTFunCall * funcall = new ASTFunCall(_currentScope, name, params);
    
    _currentScope -> childNodes.emplace_back(funcall);

}

void AbstractSyntaxTree::emplaceFunCall(const ASTFunCall & fcall) {
    
    ASTFunCall * funcall = new ASTFunCall(fcall);
    
    _currentScope -> childNodes.emplace_back(funcall);
    
}

void AbstractSyntaxTree::emplaceDeclaration(const std::string & type,
                                            const std::string & name,
                                            ASTNode * value) {
    
    checkType(type);
    
    ASTDeclaration * declaration = new ASTDeclaration(_currentScope, type, name, value);
    _currentScope -> childNodes.emplace_back(declaration);
    
    /* Check if variable doesn't already exist */
    if (_currentScope->vars[name] != "") {
        throw redefinition_of_variable(name);
    }
    
}

void AbstractSyntaxTree::emplaceClass(const std::string & className,
                                      const std::string & superClass,
                                      const std::vector<parameter> & attributes) {

    /* Structs can be defined in local scopes in C, but functions can't    */
    /* This would make implementing methods difficult, so I'm only going   */
    /* to allow class definitions in the global scope                      */
    if (_currentScope != &_globalScope) {
        throw wrong_scope("Classes can only be defined in the global scope. ");
    }
    
    for (auto & i : _dataTypes) {
        
        if (className == i) {
            throw redefinition_of_class(className);
        }
        
    }
    
    _class c;
    
    if (superClass != "") {
        
        try {
            c.attributes = _classes.at(superClass).attributes;
        }
        catch (std::out_of_range) {
            throw undefined_class(superClass);
        }
        
    }
    
    c.className = className;
    c.superClass = superClass;
    
    for (const auto & attribute : attributes) {
        
        /* Check if attribute has an existant data type */
        checkType(attribute.type);
        
        /* Check if attribute doesn't already exist */
        for (const auto & i : c.attributes) {
            if (i.name == attribute.name) {
                throw redefinition_of_attribute(i.name, className);
            }
        }
        
        c.attributes.emplace_back(attribute);
        
    }

    _dataTypes.emplace_back(className);
    _classOrder.emplace_back(className);
    _classes.emplace(className, c);

}

void AbstractSyntaxTree::leaveScope() {

    if (_currentScope -> parentScope == nullptr) {
        throw exiting_global_scope();
    }
    
    _currentScope = _currentScope -> parentScope;

}

void AbstractSyntaxTree::emplaceVariableIntoScope(const parameter & var, ASTScope * scope) {

    scope->vars[var.name] = var.type;
    
}

