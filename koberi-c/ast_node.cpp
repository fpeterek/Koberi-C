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
                               const std::string & paramValue) {
    
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
                
            case NodeType::Class:
                delete (ASTClass*)childNode;
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
                
            default:
                delete childNode;
                break;
                
        }
        
    }
    
}

ASTFunction::ASTFunction(ASTScope * parent,
                         const std::string & functionName,
                         const std::string & returnType,
                         const std::vector<parameter> & params) : scope(parent) {
    
    nodeType    = NodeType::Function;
    parentScope = parent;
    name        = functionName;
    type        = returnType;
    parameters  = params;
    
}

ASTConstruct::ASTConstruct(ASTScope * parent,
                           const std::string & construct,
                           const ASTFunCall & newCondition) : scope(parent), condition(newCondition) {

    parentScope = parent;
    this->construct = construct;
    this->condition = condition;
    
}

ASTFunCall::ASTFunCall(ASTScope * parent, const std::string & name, const std::vector<parameter> & params) {
    
    nodeType    = NodeType::FunCall;
    parentScope = parent;
    function    = name;
    parameters  = params;
    
}
