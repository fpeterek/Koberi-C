//
//  aast_node.cpp
//  koberi-c
//
//  Created by Filip Peterek on 30/10/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#include "aast_node.hpp"

std::string indent(int i) {
    
    std::string _indent;
    for (; i > 0; --i) {
        _indent += INDENT;
    }
    
    return _indent;
    
}

AASTNode::AASTNode(AASTNodeType nodeType, const std::string & dataType) : _nodeType(nodeType),
                                                                          _type(dataType) { }

AASTNodeType AASTNode::nodeType() const {
    return _nodeType;
}

std::string AASTNode::type() const {
    return _type;
}

AASTScope::AASTScope(const std::vector<AASTNode *> & body) : _body(body),
                                                             AASTNode(AASTNodeType::Scope, "") { }

AASTScope::~AASTScope() {
    for (AASTNode * node : _body) {
        delete node;
    }
}

std::string AASTScope::value(int baseIndent) const {
    
    std::stringstream stream;
    
    stream << indent(baseIndent) << "{" << "\n";
    
    for (AASTNode * node : _body) {
        const bool appendSemicolon = not (node->nodeType() == AASTNodeType::Construct or
                                          node->nodeType() == AASTNodeType::Scope or
                                          node->nodeType() == AASTNodeType::Function);
        
        stream << indent(baseIndent + 1) << node->value(baseIndent + 1) << (appendSemicolon ? ";" : "") << "\n";
    }
    
    stream << indent(baseIndent) << "}" << "\n";
    
    
    return stream.str();
    
}

AASTConstruct::AASTConstruct(const std::string & construct,
                             const AASTNode * condition,
                             const AASTScope & body) : _construct(construct),
                                                       _condition(condition),
                                                       _body(body),
                                                       AASTNode(AASTNodeType::Construct, "") { }

AASTConstruct::~AASTConstruct() {
    delete _condition;
}

std::string AASTConstruct::value(int baseIndent) const {
    
    std::stringstream stream;
    
    stream << indent(baseIndent) << _construct;
    
    if (_construct != "else") {
        stream << "(" << _condition->value(baseIndent) << ")" << "\n";
    }
    
    stream << _body.value(baseIndent + 1);
    
    return stream.str();
    
}

AASTFunction::AASTFunction(const std::string & name,
                           const std::string & type,
                           const std::vector<AASTDeclaration> & parameters,
                           const AASTScope & body) : _mangledName(name),
                                                     _parameters(parameters),
                                                     _body(body),
                                                     AASTNode(AASTNodeType::Function, type) { }

std::string AASTFunction::value(int baseIndent) const {
    
    std::stringstream stream;
    
    stream << type() << " " << _mangledName << "(";
    
    for (size_t i = 0; i < _parameters.size(); ++i) {
        
        const AASTDeclaration & param = _parameters[i];
        
        stream << param.type() << " " << param.value(baseIndent) << (i < _parameters.size() - 1 ? ", " : "");
        
    }
    
    stream << "\n" << _body.value(baseIndent + 1);
    
    return stream.str();
    
}

AASTClass::AASTClass(const std::string & name,
                     const std::vector<AASTDeclaration> & attributes) : _name(name),
                                                                        _attributes(attributes),
                                                                        AASTNode(AASTNodeType::Class, name) { }

std::string AASTClass::value(int baseIndent) const {
    
    std::stringstream stream;
    
    stream << "typedef struct " << _name << "\n" << "{" << "\n";
    
    for (const AASTDeclaration & attribute : _attributes) {
        stream << indent(baseIndent) << attribute.value(baseIndent + 1) << ";";
    }
    
    stream << "} " << _name << ";" << "\n";
    
    return stream.str();
    
}

AASTFuncall::AASTFuncall(const std::string & name,
                         const std::string & type,
                         const std::vector<AASTNode *> parameters) : _mangledName(name),
                                                                     _parameters(parameters),
                                                                     AASTNode(AASTNodeType::Funcall, type) { }

AASTFuncall::~AASTFuncall() {
    
    for (AASTNode * parameter : _parameters) {
        delete parameter;
    }
    
}

std::string AASTFuncall::value(int baseIndent) const {
    
    std::stringstream stream;
    
    stream << _mangledName << "(";
    
    for (size_t i = 0; i < _parameters.size(); ++i) {
        
        const AASTNode * param = _parameters[i];
        
        stream << param->value(baseIndent + 1) << (i < _parameters.size() - 1 ? ", " : "");
        
    }
    
    stream << ")";
    
    return stream.str();
    
}

AASTValue::AASTValue(const std::string & value,
                     const std::string & type) : _value(value),
                                                 AASTNode(AASTNodeType::Value, type) { }

std::string AASTValue::value(int baseIndent) const {
    
    return _value;
    
}

AASTDeclaration::AASTDeclaration(const std::string & name,
                                 const std::string & type,
                                 const AASTNode * value) : _name(name),
                                                           _value(value),
                                                           AASTNode(AASTNodeType::Declaration, type) { }

AASTDeclaration::~AASTDeclaration() {
    delete _value;
}

std::string AASTDeclaration::value(int baseIndent) const {
    
    std::stringstream stream;
    
    stream << type() << " " << _name;
    
    if (_value != nullptr) {
        stream << " = " << _value->AASTNode::value(baseIndent + 1);
    }
    
    stream << ";";
    
    return stream.str();
    
}

AASTOperator::AASTOperator(const std::string & op,
                           const std::string & type,
                           const std::vector<AASTNode *> parameters) : _operator(op),
                                                                       _parameters(parameters),
                                                                       AASTNode(AASTNodeType::Operator, type) { }

AASTOperator::~AASTOperator() {
    
    for (AASTNode * parameter : _parameters) {
        delete parameter;
    }
    
}

std::string AASTOperator::value(int baseIndent) const {
    
    std::stringstream stream;
    
    std::vector<std::string> values;
    
    return stream.str();
    
}





