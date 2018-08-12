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

/* Translates type from Kobeři-C type to C type */
std::string translateType(const std::string & type) {
    
    std::string t = type;
    const bool isPtr = syntax::isPointerType(t);
    if (isPtr) {
        t.pop_back();
    }
    
    if (t == "int") {
        t = syntax::intType;
    } else if (t == "uint") {
        t = syntax::uintType;
    } else if (t == "num") {
        t = syntax::floatType;
    }
    
    if (isPtr) {
        t.append( std::string(1, syntax::pointerChar) );
    }
    
    return t;
    
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
    
    stream << "\n" << indent(baseIndent) << "{" << "\n";
    
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
                             const AASTScope * body) : _construct(construct),
                                                       _condition(condition),
                                                       _body(body),
                                                       AASTNode(AASTNodeType::Construct, "") { }

AASTConstruct::~AASTConstruct() {
    delete _condition;
}

std::string AASTConstruct::value(int baseIndent) const {
    
    std::stringstream stream;
    
    std::string construct = _construct;
    
    if (_construct == "elif") {
        construct = "else if";
    } else if (_construct == "dowhile") {
        construct = "do";
    }
    
    stream << indent(baseIndent - 1) << construct;
    
    if (construct != "else" and construct != "do") {
        stream << " (" << _condition->value(0) << ")";
    }
    
    stream << _body->value(baseIndent);
    
    if (_construct == "dowhile") {
        stream << indent(baseIndent) << "while (" << _condition->value(0) << ");";
    }
    
    return stream.str();
    
}

AASTFunction::AASTFunction(const std::string & name,
                           const std::string & type,
                           const std::vector<AASTDeclaration> & parameters,
                           const AASTScope * body) : _mangledName(name),
                                                     _parameters(parameters),
                                                     _body(body),
                                                     AASTNode(AASTNodeType::Function, type) { }

std::string AASTFunction::value(int baseIndent) const {
    
    std::stringstream stream;
    
    stream << declaration() << _body->value(baseIndent);
    
    return stream.str();
    
}

std::string AASTFunction::declaration() const {
    
    std::stringstream stream;
    
    stream << translateType(type()) << " " << _mangledName << "(";
    
    for (size_t i = 0; i < _parameters.size(); ++i) {
        
        const AASTDeclaration & param = _parameters[i];
        
        stream << param.value() << (i < _parameters.size() - 1 ? ", " : "");
        
    }
    
    stream << ")";
    
    return stream.str();
    
}

AASTClass::AASTClass(const std::string & name,
                     const std::vector<AASTDeclaration> & attributes,
                     const VTable & vt) : _name(name),
                                          _attributes(attributes),
                                          _vtable(vt),
                                          AASTNode(AASTNodeType::Class, name) { }

std::string AASTClass::value(int baseIndent) const {
    
    std::stringstream stream;
    
    stream << "typedef struct " << _name << "\n" << "{" << "\n";
    
    stream << indent(baseIndent + 1) << "void (**vtable)(void)" << ";\n";
    
    for (const AASTDeclaration & attribute : _attributes) {
        stream << indent(baseIndent + 1) << attribute.value(baseIndent + 1) << ";\n";
    }
    
    stream << "} " << _name << ";" << "\n";
    
    return stream.str();
    
}

std::string AASTClass::vtable() const {
    
    /* Inserting whitespace after { so when trailing commas are being removed, the whitespace */
    /* Character will be removed instead of the brace { */
    std::string table = "void (*" + NameMangler::vtableName(_name) +"[])(void) = { \n";
    
    std::vector<std::string> methods;
    
    for (size_t i = 0; i < _vtable.size(); ++i) {
        methods.emplace_back("");
    }
    
    for (const auto & m : _vtable) {
        
        const std::string & mname = m.first;
        const _method & method = m.second;
        
        methods[method.pointerIndex] = "(void (*)(void))" + NameMangler::premangleMethodName(mname, method.className);
        
    }
    
    for (const auto & m : methods) {
        table += indent(1) + m + ",\n";
    }
    
    /* Remove trailing whitespace and comma */
    table.pop_back(); table.pop_back();
    
    table += "\n}";
    
    return table;
    
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
                     const std::string & type,
                     bool assignable) : _value(value), _isAssignable(assignable),
                                        AASTNode(AASTNodeType::Value, type) { }

bool AASTValue::isAssignable() {
    return _isAssignable;
}

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
    
    stream << translateType(type()) << " " << _name;
    
    if (_value != nullptr) {
        stream << " = " << _value->value(baseIndent + 1);
    }
    
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

void inlineC(std::stringstream & stream, const std::vector<parameter> & values, const int indentLevel);

void print(std::stringstream & stream, const std::vector<parameter> & values, const int indentLevel);

void _new(std::stringstream & stream, const std::vector<parameter> & values, const int indentLevel);

std::string AASTOperator::value(int baseIndent) const {
    
    std::stringstream stream;
    
    std::vector<parameter> values;
    for (AASTNode * param : _parameters) {
        values.emplace_back(param->value(baseIndent + 1), param->type());
    }
    
    if (_operator == "_c") {
        inlineC(stream, values, baseIndent);
    }
    
    else if (_operator == "new") {
        _new(stream, values, baseIndent);
    }
    
    else if (_operator == "print") {
        print(stream, values, baseIndent);
    }
    
    else if (not values.size()) {
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

void print(std::stringstream & stream, const std::vector<parameter> & values, const int indentLevel) {
    
    size_t c = 0;
    
    for (auto & p : values) {
        
        /* First value is indented automatically, others need to be indented manually */
        if (c) {
            stream << indent(indentLevel);
        }
        
        if (p.type == syntax::pointerForType("char")) {
            stream << "fputs(" + p.value + ", stdout)";
        } else if (p.type == "char" or p.type == "uchar") {
            stream << "putchar(" + p.value + ")";
        } else if (p.type == "int") {
            stream << "printf(\"%lld\", " + p.value + ")";
        } else if (p.type == "uint") {
            stream << "printf(\"%ull\", " + p.value + ")";
        } else if (p.type == "num") {
            stream << "printf(\"%f\", " + p.value + ")";
        }
        
        ++c;
        
        if (c != values.size()) {
            stream << ";\n";
        }
        
    }
    
    
}

/* Inline C requires escaped escape sequences to use escape sequences in inline C  */
/* because inline C is written as string literals                                  */
/* Remove one level of escaping to allow usage of escape sequences inside inline C */
void removeEscape(std::string & str) {
    
    std::stringstream ss;
    
    bool isEscape = false;
    for (char c : str) {
        
        if (c == '\\' and not isEscape) {
            isEscape = true;
        } else {
            isEscape = false;
        }
        
        if (isEscape) {
            continue;
        }
        
        ss << c;
        
    }
    
    str = ss.str();
    
}

void inlineC(std::stringstream & stream, const std::vector<parameter> & values, const int indentLevel) {
    
    size_t c = 0;
    
    for (auto & i : values) {
        
        /* First value is indented automatically, others need to be indented manually */
        if (c) {
            stream << indent(indentLevel);
        }
        
        /* Trim quotes " */
        std::string val = i.value.c_str() + 1;
        val.pop_back();
        removeEscape(val);
        
        stream << val;
        
        ++c;
        
        if (c != values.size()) {
            stream << "\n";
        }
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
    else if (op == "*") {
        
        if (not syntax::isPointerType(parameter.type)) {
            stream << parameter.value;
        } else {
            stream << "(*" << parameter.value << ")";
        }
        
    }

    else {
        stream << op << "( " << parameter.value << " )";
    }
    
}

void _new(std::stringstream & stream, const std::vector<parameter> & values, const int indentLevel) {
    
    std::string type = translateType(values.front().value);
    //stream << "((" + parameter.value +  "*)malloc(sizeof(" + parameter.value + ")))";
    stream << "(" << syntax::memoryAlloc << "(sizeof(" + type + "), ";
    stream << values.back().value + "));";
    
}

/* % operator can't be used on floats in C, fmod() needs to be called instead */
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
    
    std::string str = "(";
    
    for (size_t i = 0; i < parameters.size(); ++i) {
        str += parameters[i].value + " " + op + " ";
    }
    
    /* Pop back two spaces and operator, which were appended at the end of last loop */
    for (char i = 0; i < op.length() + 2; ++i) {
        str.pop_back();
    }
    
    str += ")";
    stream << str;
    
}

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
        stream << "((" << translateType(_desiredType) << ")(void *)" << _value->value() << ")";
    } else {
        stream << "((" << translateType(_desiredType) << ")" << _value->value() << ")";
    }
    
    return stream.str();
    
}

