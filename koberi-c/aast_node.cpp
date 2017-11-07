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

AASTNode::~AASTNode() {
    
}

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
    
    stream << declaration() << "\n" << _body.value(baseIndent + 1);
    
    return stream.str();
    
}

std::string AASTFunction::declaration() const {
    
    std::stringstream stream;
    
    stream << type() << " " << _mangledName << "(";
    
    for (size_t i = 0; i < _parameters.size(); ++i) {
        
        const AASTDeclaration & param = _parameters[i];
        
        stream << param.type() << " " << param.value() << (i < _parameters.size() - 1 ? ", " : "");
        
    }
    
    stream << ")";
    
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
        stream << " = " << _value->value(baseIndent + 1);
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

const std::string & AASTOperator::getOperator() const {
    
    return _operator;
    
}

void unaryOperator(std::stringstream & stream, const std::string & op, const parameter & parameter);

void binaryOperator(std::stringstream & stream,
                    const std::string & op,
                    std::vector<parameter> & parameters);

void inlineC(std::stringstream & stream, const std::vector<parameter> values, const int indentLevel);

std::string AASTOperator::value(int baseIndent) const {
    
    std::stringstream stream;
    
    std::vector<parameter> values;
    for (AASTNode * param : _parameters) {
        values.emplace_back(param->value(baseIndent + 1), param->type());
    }
    
    if (_operator == "_c") {
        inlineC(stream, values, baseIndent);
    }
    
    if (not values.size()) {
        stream << _operator;
    }
    
    else if (values.size() == 1) {
        unaryOperator(stream, _operator, values[0]);
    }
    
    else {
        binaryOperator(stream, _operator, values);
    }
    
    return stream.str();
    
}

void inlineC(std::stringstream & stream, const std::vector<parameter> values, const int indentLevel) {
    
    for (auto & i : values) {
        stream << indent(indentLevel) << i.value << "\n";
    }
    
}

void unaryOperator(std::stringstream & stream, const std::string & op, const parameter & parameter) {
    
    if (op == "-") {
        stream << "((" << parameter.value << ") * (-1))";
    }
    else if (op == "&") {
        
        if (syntax::isPointerType(parameter.type)) {
            stream << parameter.value;
        } else {
            stream << "(&" << parameter.value << ")";
        }
        
    }
    else if (op == "new") {
        stream << "((" + parameter.value +  ")malloc(sizeof(" + parameter.value + ")))";
    }
    else {
        stream << op << "( " << parameter.value << " )";
    }
    
}

void fmodOperator(std::stringstream & stream,
                  std::vector<parameter> & parameters) {
    
    if ( not parameters.size() ) {
        return;
    }
    if ( parameters.size() == 1 ) {
        stream << parameters[0].value;
        return;
        
    }
    
    parameter val;
    val.value = "fmod(" + parameters[0].value + ", " + parameters[1].value + ")";
    
    auto iter = parameters.begin();
    parameters.erase(iter);
    parameters.erase(iter);
    parameters.insert(iter, val.value);
    
    return fmodOperator(stream, parameters);
    
}

void comparison(std::stringstream & stream,
                const std::string & op,
                const std::vector<parameter> & parameters) {
    
    for (size_t i = 1; i < parameters.size(); ++i) {
        
        stream << parameters[i - 1].value << " " << op << " " <<
                  parameters[i].value << ((i < parameters.size() - 1) ? " && " : "");
    
    }
    
}

void set(std::stringstream & stream,
         const std::vector<parameter> & parameters) {
    
    stream << parameters[0].value << " = " << parameters[1].value;
    
}

void dereference(std::stringstream & stream, const AASTNode * parameter) {
    
    
    
}

void binaryOperator(std::stringstream & stream,
                    const std::string & op,
                    std::vector<parameter> & parameters) {
    
    if (op == "fmod") {
        return fmodOperator(stream, parameters);
    }
    
    if (op == "<" or op == ">" or op == "<=" or op == ">=" or op == "==" or op == "!=") {
        return comparison(stream, op, parameters);
    }
    
    if (op == "=") {
        return set(stream, parameters);
    }
    
}

/* old expr::binary_operator
 
 if (op == "mod") {
 
 for (auto & param : params) {
 
 // If at least one of the parameters is a floating point number, call fmod()
if (param.type == "num") {
    return expr::numMod(params);
}

}

}

if (op == ">" or op == ">=" or op == "<" or op == "<=" or op == "equals" or op == "not_eq") {
    return comparison(op, params);
}

if (op == "set" and params.size() != 2) {
    std::string str = "(set";
    for (auto & i : params) {
        str += " " + i.value;
    }
    str += ")";
    throw invalid_operator(str);
}

parameter val;
val.type = "void";

const std::string oper = binary_operators_map.at(op);

if (op == "set" and params[0].type.back() == syntax::pointerChar and params[1].type.back() != syntax::pointerChar) {
    params[0].value = "(*" + params[0].value + ")";
} else if (op == "set" and params[0].type.back() != syntax::pointerChar and params[1].type.back() == syntax::pointerChar) {
    
    if (params[0].value.substr(0, 2) == "(*" and params[0].value.back() == ')') {
        params[0].value = params[0].value.substr(2);
        params[0].value.pop_back();
    } else {
        throw invalid_parameter("Cannot assign value of type " + params[1].type +
                                " to variable of type " + params[0].type);
    }
    
}

val.value = "(" + params[0].value;
val.type = params[0].type;

for ( int i = 1; i < params.size() - 1; ++i ) {
    
    val.value += " " + oper + " " + params[i].value;
    
    if (params[i].value == "num") {
        val.value = "num";
    }
    
}
val.value += " " + oper + " " + params.back().value + ")";

return val;
 
 */

AASTCast::AASTCast(const AASTNode * value, const std::string desiredType) :
                _value(value),
                _desiredType(desiredType),
                AASTNode(AASTNodeType::Cast, desiredType) { }

AASTCast::~AASTCast() {
    delete _value;
}

std::string AASTCast::value(int baseIndent) const {
    
    std::stringstream stream;
    
    if (syntax::isPointerType(_desiredType)) {
        stream << "((" << _desiredType << ")(void *)" << _value->value() << ")";
    } else {
        stream << "((" << _desiredType << ")" << _value->value() << ")";
    }
    
    return stream.str();
    
}
