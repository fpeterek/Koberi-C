//
//  ast_node.cpp
//  koberi-c
//
//  Created by Filip Peterek on 02/04/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#include "ast_node.hpp"

ASTDeclaration::ASTDeclaration(ASTScope * parent,
                               const std::string & paramType,
                               const std::string & paramName,
                               ASTNode * paramValue) {
    
    nodeType    = NodeType::Declaration;
    parentScope = parent;
    type        = paramType;
    name        = paramName;
    value       = paramValue;
    
}

ASTScope::ASTScope(ASTScope * parent) {
    
    nodeType    = NodeType::Scope;
    parentScope = parent;
    
}

ASTScope::~ASTScope() {
    
    for (ASTNode * childNode : childNodes) {
        
        switch (childNode -> nodeType) {
                
            case NodeType::Scope:
                delete (ASTScope*)childNode;
                break;
                
            case NodeType::FunCall:
                delete (ASTFunCall*)childNode;
                break;
                
            case NodeType::Declaration:
                delete (ASTDeclaration*)childNode;
                break;
                
            case NodeType::Function:
                delete (ASTFunction*)childNode;
                break;
                
            case NodeType::Construct:
                delete (ASTConstruct*)childNode;
                break;
                
            /* These two cases should never occur, but I'm putting them here just to be sure */
            /* Can't risk creating memory leaks if someone emplaces these somewhere          */
            case NodeType::Variable:
                delete (ASTVariable*)childNode;
                break;
                
            case NodeType::Literal:
                delete (ASTLiteral*)childNode;
                break;
                
            default:
                delete childNode;
                break;
                
        }
        
    }
    
}

ASTFunction::ASTFunction(ASTScope * parent,
                         const std::string & functionName,
                         const std::string & returnType,
                         const std::vector<parameter> & params) : ASTScope(parent) {
    
    nodeType    = NodeType::Function;
    name        = functionName;
    type        = returnType;
    parameters  = params;
    
}

ASTConstruct::ASTConstruct(ASTScope * parent,
                           const std::string & construct,
                           const ASTFunCall & newCondition) : ASTScope(parent), condition(newCondition) {

    nodeType = NodeType::Construct;
    this->construct = construct;
    this->condition = condition;
    
}

ASTFunCall::ASTFunCall(ASTScope * parent, const std::string & name, const std::vector<ASTNode *> & params) {
    
    nodeType    = NodeType::FunCall;
    parentScope = parent;
    function    = name;
    parameters  = params;
    
}

ASTFunCall::~ASTFunCall() {
    
    for (ASTNode * param : parameters) {
        
        switch (param -> nodeType) {
                
            case NodeType::Variable:
                delete (ASTVariable*)param;
                break;
                
            case NodeType::Literal:
                delete (ASTLiteral*)param;
                break;
                
            /* These cases should never occur, but I'd rather be safe than sorry */
                
            case NodeType::Scope:
                delete (ASTScope*)param;
                break;
                
            case NodeType::FunCall:
                delete (ASTFunCall*)param;
                break;
                
            case NodeType::Declaration:
                delete (ASTDeclaration*)param;
                break;
                
            case NodeType::Function:
                delete (ASTFunction*)param;
                break;
                
            case NodeType::Construct:
                delete (ASTConstruct*)param;
                break;
                
            default:
                delete param;
                break;
                
        }
        
    }
    
}

ASTLiteral::ASTLiteral(const parameter & literal) {

    nodeType = NodeType::Literal;
    type     = literal.type;
    value    = literal.value;
    
    /* Parent scope shouldn't be accessed here, so I'm setting it to nullptr */
    parentScope = nullptr;
    
}

ASTLiteral::ASTLiteral(const std::string & literalType, const std::string & literalValue) {
    
    nodeType = NodeType::Literal;
    type     = literalType;
    value    = literalValue;
    
    /* Parent scope shouldn't be accessed here, so I'm setting it to nullptr */
    parentScope = nullptr;
    
}

ASTVariable::ASTVariable(const std::string & variableName) {

    nodeType = NodeType::Variable;
    name     = variableName;
    
    /* Parent scope shouldn't be accessed here, so I'm setting it to nullptr */
    parentScope = nullptr;
    
}

ASTVariable * ASTVariable::createVariable(const std::string & variableName) {
    
    ASTVariable * var = new ASTVariable(variableName);
    
    return var;
    
}

ASTLiteral * ASTLiteral::createLiteral(const std::string & type,
                                       const std::string & value) {
    
    ASTLiteral * literal = new ASTLiteral(type, value);
    
    return literal;
    
}

ASTFunCall * ASTFunCall::createFunCall(const std::string & name,
                                       const std::vector<ASTNode *> & params) {
    
    ASTFunCall * funcall = new ASTFunCall(nullptr, name, params);
    
    return funcall;
    
}
