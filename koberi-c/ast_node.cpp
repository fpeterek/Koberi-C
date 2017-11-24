//
//  ast_node.cpp
//  koberi-c
//
//  Created by Filip Peterek on 02/04/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#include "ast_node.hpp"

ASTNode::~ASTNode() { }

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
        
        delete childNode;
        
    }
    
}

ASTFunction::ASTFunction(ASTScope * parent,
                         const std::string & functionName,
                         const std::string & returnType,
                         const std::vector<parameter> & params,
                         const std::string & className) : ASTScope(parent) {
    
    nodeType    = NodeType::Function;
    name        = functionName;
    type        = returnType;
    parameters  = params;
    this->className = className;
    
}

ASTConstruct::ASTConstruct(ASTScope * parent,
                           const std::string & construct,
                           ASTNode * newCondition) : ASTScope(parent) {

    nodeType = NodeType::Construct;
    this->construct = construct;
    this->condition = newCondition;
    
}

ASTConstruct::~ASTConstruct() {
    
    delete condition;
    
}

ASTFunCall::ASTFunCall(ASTScope * parent,
                       const std::string & name,
                       const std::vector<ASTNode *> & params,
                       ASTMemberAccess * object) {
    
    nodeType    = NodeType::FunCall;
    parentScope = parent;
    function    = name;
    this->object = object;

    for (ASTNode * param : params) {
        
        switch (param -> nodeType) {
                
            case NodeType::Variable:
                parameters.emplace_back( new ASTVariable( *( (ASTVariable*)param ) ) );
                break;
                
            case NodeType::Literal:
                parameters.emplace_back( new ASTLiteral( *( (ASTLiteral*)param ) ) );
                break;
                
            case NodeType::FunCall:
                parameters.emplace_back( new ASTFunCall( *( (ASTFunCall*)param ) ) );
                break;
                
            case NodeType::MemberAccess:
                parameters.emplace_back( new ASTMemberAccess ( *( (ASTMemberAccess*)param ) ) );
                break;
                
                /* These cases should never occur, but I'd rather be safe than sorry */
                
            case NodeType::Scope:
                parameters.emplace_back( new ASTScope( *( (ASTScope*)param ) ) );
                break;
                
            case NodeType::Declaration:
                parameters.emplace_back( new ASTDeclaration( *( (ASTDeclaration*)param ) ) );
                break;
                
            case NodeType::Function:
                parameters.emplace_back( new ASTFunction( *( (ASTFunction*)param ) ) );
                break;
                
            case NodeType::Construct:
                parameters.emplace_back( new ASTConstruct( *( (ASTConstruct*)param ) ) );
                break;
                
            default:
                delete param;
                break;
                
        }
        
    }
    
}

ASTFunCall::ASTFunCall(const ASTFunCall & orig) {
    
    nodeType = NodeType::FunCall;
    parentScope = orig.parentScope;
    function = orig.function;
    if (orig.object != nullptr) {
        object = new ASTMemberAccess(*orig.object);
    } else {
        object = nullptr;
    }
    
    for (ASTNode * param : orig.parameters) {
        
        switch (param -> nodeType) {
                
            case NodeType::Variable:
                parameters.emplace_back( new ASTVariable( *( (ASTVariable*)param ) ) );
                break;
                
            case NodeType::Literal:
                parameters.emplace_back( new ASTLiteral( *( (ASTLiteral*)param ) ) );
                break;
                
            case NodeType::FunCall:
                parameters.emplace_back( new ASTFunCall( *( (ASTFunCall*)param ) ) );
                break;
                
            case NodeType::MemberAccess:
                parameters.emplace_back( new ASTMemberAccess ( *( (ASTMemberAccess*)param ) ) );
                break;
                
                /* These cases should never occur, but I'd rather be safe than sorry */
                
            case NodeType::Scope:
                parameters.emplace_back( new ASTScope( *( (ASTScope*)param ) ) );
                break;
                
            case NodeType::Declaration:
                parameters.emplace_back( new ASTDeclaration( *( (ASTDeclaration*)param ) ) );
                break;
                
            case NodeType::Function:
                parameters.emplace_back( new ASTFunction( *( (ASTFunction*)param ) ) );
                break;
                
            case NodeType::Construct:
                parameters.emplace_back( new ASTConstruct( *( (ASTConstruct*)param ) ) );
                break;
                
            default:
                delete param;
                break;
                
        }
        
    }
    
}

ASTFunCall::~ASTFunCall() {
    
    if (object != nullptr) {
        delete object;
    }
    
    for (ASTNode * param : parameters) {
        
        delete param;
        
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

ASTVariable::ASTVariable(const std::string & variableName, ASTScope * parent) {

    nodeType = NodeType::Variable;
    name     = variableName;
    
    /* Holds pointer to the scope in which the variable was accessed */
    parentScope = parent;
    
}

ASTVariable * ASTVariable::createVariable(const std::string & variableName, ASTScope * parent) {
    
    ASTVariable * var = new ASTVariable(variableName, parent);
    
    return var;
    
}

void ASTMemberAccess::check() {
    
    if (not accessOrder.size()) {
        return;
    }
    
    NodeType type = (*accessOrder[0]).nodeType;
    if (type != NodeType::Variable and type != NodeType::FunCall) {
        throw invalid_attribute_access("First parameter of [] operator must be a variable or function call.");
    }
    
    for (size_t i = 1; i < accessOrder.size(); ++i) {
        
        type = (*accessOrder[i]).nodeType;
        if (type != NodeType::Variable) {
            throw invalid_attribute_access("Parameters of [] operator beyond the first operator must be valid identifiers. ");
        }
        
    }
    
}

ASTMemberAccess::ASTMemberAccess(const std::vector<ASTNode *> & accessOrder, ASTScope * parent) {
    
    nodeType = NodeType::MemberAccess;
    parentScope = parent;
    
    this->accessOrder = accessOrder;
    
    check();
    
}

ASTMemberAccess::ASTMemberAccess(const ASTMemberAccess & orig) {
    
    nodeType = NodeType::MemberAccess;
    parentScope = orig.parentScope;
    
    for (ASTNode * node : orig.accessOrder) {
        
        switch (node -> nodeType) {
                
            case NodeType::Variable:
                accessOrder.emplace_back( new ASTVariable( *((ASTVariable*)node) ) );
                break;
                
            case NodeType::Literal:
                accessOrder.emplace_back( new ASTLiteral( *((ASTLiteral*)node) ) );
                break;
                
            case NodeType::MemberAccess:
                accessOrder.emplace_back( new ASTMemberAccess( *((ASTMemberAccess*)node) ) );
                break;
                
            case NodeType::FunCall:
                accessOrder.emplace_back( new ASTFunCall( *((ASTFunCall*)node) ) );
                break;
                
            case NodeType::Scope:
                accessOrder.emplace_back( new ASTScope( *((ASTScope*)node) ) );
                break;
                
            case NodeType::Declaration:
                accessOrder.emplace_back( new ASTDeclaration( *((ASTDeclaration*)node) ) );
                break;
                
            case NodeType::Function:
                accessOrder.emplace_back( new ASTFunction( *((ASTFunction*)node) ) );
                break;
                
            case NodeType::Construct:
                accessOrder.emplace_back( new ASTConstruct( *((ASTConstruct*)node) ) );
                break;
                
            default:
                break;
                
        }
        
    }
    
}


ASTMemberAccess::~ASTMemberAccess() {
    
    for (ASTNode * param : accessOrder) {
        
        delete param;
        
    }
    
}

ASTLiteral * ASTLiteral::createLiteral(const std::string & type,
                                       const std::string & value) {
    
    ASTLiteral * literal = new ASTLiteral(type, value);
    
    return literal;
    
}

ASTFunCall * ASTFunCall::createFunCall(ASTScope * parent,
                                       const std::string & name,
                                       const std::vector<ASTNode *> & params,
                                       ASTMemberAccess * object) {
    
    ASTFunCall * funcall = new ASTFunCall(parent, name, params, object);
    
    return funcall;
    
}
