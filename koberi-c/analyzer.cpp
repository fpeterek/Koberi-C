//
//  Analyzer.cpp
//  koberi-c
//
//  Created by Filip Peterek on 30/10/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#include "analyzer.hpp"

Analyzer::Analyzer(TraversableAbstractSyntaxTree & ast, AnalyzedAbstractSyntaxTree & aast) : _ast(ast), _aast(aast) {
    
}

void Analyzer::kobericMainCheck() {
    
    std::string type;
    
    try {
        /* If main doesn't exist, exception is thrown */
        type = _ast.getFunctionReturnType(NameMangler::mangleName("main", std::vector<parameter>()));
    }
    catch (const std::exception & e) {
        throw missing_main();
    }
    
    /* If main exists, check return type */
    if (type != "int") {
        throw invalid_main();
    }
    
}

void Analyzer::analyzeClasses() {
    
    if (expr::isVerbose()) {
        std::cout << "\n" << "Analyzing classes..." << "\n" << std::endl;
    }
    
    
    const std::unordered_map<std::string, _class> & classes = _ast.getClasses();
    const std::vector<std::string> & classOrder = _ast.getClassOrder();
    
    for (const auto & c : classOrder) {
        
        const _class & cls = classes.at(c);
        checkIdIsValid(cls.className);

        std::vector<AASTDeclaration> attributes;
        
        for (const auto & attr : cls.attributes) {
            
            checkIdIsValid(attr.name);
            attributes.emplace_back(AASTDeclaration(attr.name, attr.type, nullptr));
            
        }
        
        AASTClass * _class = new AASTClass(cls.className, attributes);
        _aast.emplaceClass(_class);
        
    }
    
}

void Analyzer::analyzeGlobalVars() {
    
    const size_t nodeCount = _ast.getNodeCount();
    
    for (size_t i = 0; i < nodeCount; ++i) {
        
        ASTNode * node = _ast.getNodePtr(i);
        
        if (node->nodeType == NodeType::Declaration) {
            
            ASTDeclaration * decl = (ASTDeclaration*)node;
            checkIdIsValid(decl->name);
            
            AASTDeclaration * declaration = new AASTDeclaration(decl->name, decl->type, nullptr);
            _aast.emplaceGlobalDeclaration(declaration);
            
        }
        
    }
    
}

void Analyzer::checkIdIsValid(const std::string & id) {
    
    auto isLetterOrUnderscore = [] (const char c) -> bool {
        return ('A' <= c and c <= 'Z') or ('a' <= c and c <= 'z') or c == '_';
    };
    
    auto isValidIdChar = [&] (const char c) -> bool {
        return isLetterOrUnderscore(c) or ('0' <= c and c <= '9');
    };
    
    if (not isLetterOrUnderscore(id[0])) {
        throw invalid_identifier(id);
    }
    
    for (size_t i = 1; i < id.length(); ++i) {
        
        if (not isValidIdChar(id[i])) {
            throw invalid_identifier(id);
        }
        
    }
    
    if (contains(expr::keywords, id)) {
        throw invalid_identifier(id);
    }
    
}

parameter Analyzer::getVariable(ASTVariable & variable) {
    
    parameter var;
    
    var.name = variable.name;
    var.type = _ast.getVarType(variable.name, variable.parentScope);
    
    return var;
    
}

void Analyzer::analyzeFunctions() {
    
    ASTNode * node;
    
    for (size_t i = 0; i < _ast.getNodeCount(); ++i) {
        
        node = _ast.getNodePtr(i);
        
        if (node->nodeType == NodeType::Function) {
            
            ASTFunction & function = *((ASTFunction*)node);
            analyzeFunction(function);
            
        }
        
    }
    
}

void Analyzer::analyzeFunction(ASTFunction & function) {
    
    _functionName = function.name;
    _functionType = function.type;
    _declarations = std::vector<ASTDeclaration *>();
    
    const std::vector<parameter> & params = function.parameters;
    
    if (expr::isVerbose()) {
        std::cout << "Analyzing function " << "(" << _functionType << " " << _functionName << "(";
        
        for (auto & p : params) {
            std::cout << p.type << " " << p.name << " ";
        }
        
        std::cout << "))" << std::endl;
    }
    
    /* Mangle name by parameters and class name */
    std::string name = NameMangler::mangleName(function.name, params);
    if (function.className != "") {
        name = NameMangler::premangleMethodName(name, function.className);
    }
    
    std::vector<AASTDeclaration> parameters;
    
    /* If function is a member function, pass pointer to self as first parameter */
    if (function.className != "") {
        parameters.emplace_back(AASTDeclaration("self", syntax::pointerForType(function.className), nullptr));
    }
    
    for (size_t i = 0; i < params.size(); ++i) {
        parameters.emplace_back(AASTDeclaration(params[i].name, params[i].type, nullptr));
    }
    
    AASTFunction * analyzedFunction = new AASTFunction(name,
                                                       function.type,
                                                       parameters,
                                                       analyzeScope(function.childNodes));
    
    _aast.emplaceFunction(analyzedFunction);
    
}

AASTScope * Analyzer::analyzeScope(std::vector<ASTNode *> scopeNodes) {
    
    std::vector<AASTNode *> body;
    
    /* Translate all body nodes */
    for (ASTNode * node : scopeNodes) {
        
        if (node->nodeType == NodeType::Declaration) {
            _declarations.emplace_back((ASTDeclaration *)node);
        }
        
        body.emplace_back(analyzeFunctionNode(node));
    }

    /* After translating all body nodes, call destructors on scoped objects */
    std::vector<AASTFuncall *> destructors = destructScopedObjects(scopeNodes);
    for (AASTFuncall* i : destructors) {
        _declarations.pop_back();
        body.emplace_back(i);
    }
    
    return new AASTScope(body);
    
}

AASTNode * Analyzer::analyzeFunctionNode(ASTNode * node) {
    
    AASTNode * functionNode = nullptr;
    
    if (node->nodeType == NodeType::FunCall) {
        
        ASTFunCall * funcall = (ASTFunCall*)node;
        functionNode = analyzeFunCall(*funcall);
        
    } else if (node->nodeType == NodeType::Construct) {
        
        ASTConstruct * c = (ASTConstruct*)node;
        functionNode = analyzeConstruct(*c);
        
    } else if (node->nodeType == NodeType::Declaration) {
        
        ASTDeclaration * d = (ASTDeclaration*)node;
        functionNode = analyzeDeclaration(*d);
        
    } else {
        
        throw invalid_statement(currentFunction());
        
    }
    
    return functionNode;
    
}

AASTNode * Analyzer::analyzeFunCall(ASTFunCall & funcall) {
    
    std::string name = funcall.function;
    std::string type;
    
    /* Only used if function is a member function   */
    /* It needs to be declared at this scope though */
    method m;
    
    /* Casting and new need to be handled separately, because it contains class names */
    if (name == "cast" and funcall.parameters.size() == 2 and funcall.object == nullptr) {
        
        if (funcall.parameters[1]->nodeType == NodeType::Variable and
            _ast.isDataType(((ASTVariable*)funcall.parameters[1])->name)) {
            
            AASTNode * p = getFuncallParameter(funcall.parameters[0]);
            
            type = ((ASTVariable*)funcall.parameters[1])->name;
            
            return cast(p, type);
            
        }
        
    }
    
    if (name == "new" and funcall.parameters.size() == 1 and funcall.object == nullptr) {
        type = ((ASTVariable*)funcall.parameters[0])->name;
        return (AASTNode *)newObject(type);
    }
    
    std::vector<AASTNode *> params;
    
    for (auto & param : funcall.parameters) {
        params.emplace_back(getFuncallParameter(param));
    }
    
    for (AASTNode * param : params) {
        if (param->type() == "") {
            throw invalid_parameter(currentFunction(), funcall.function, param->value());
        }
    }
    
    if (name == "delete" and funcall.object == nullptr and params.size() == 1) {
        return (AASTNode *)deleteObject(params[0]);
    }
    if (name == "return" and funcall.object == nullptr) {
        return (AASTNode *)analyzeReturn(params);
    }
    if (expr::isOperator(name) and funcall.object == nullptr) {
        return (AASTNode *)analyzeOperator(name, params);
    }
    if (name == "print" and funcall.object == nullptr) {
        return (AASTNode *)analyzePrint(params);
    }
    if (name == "_c" and funcall.object == nullptr) {
        return (AASTNode *)inlineC(params, funcall);
    }
    
    {
        /* Mangle name and get return type, if function doesn't exist, and exception should be thrown */
        /* If function exists, create valid C function call from provided parameters                  */
        std::vector<std::string> paramTypes;
        for (AASTNode *& node : params) {
            
            node = expr::dereferencePtr(node);
            paramTypes.emplace_back(node->type());
        }
        name = NameMangler::mangleName(name, paramTypes);
    }
    
    if (funcall.object != nullptr) {
        
        AASTValue object = analyzeMemberAccess(*funcall.object);
        
        /* Use & operator to access address of self */
        std::vector<AASTNode *> param = { (AASTNode *)new AASTValue(object) };
        AASTOperator * op = analyzeOperator("&", param);
        
        params.insert(params.begin(), op);
        
        std::string _class = object.type();
        if (syntax::isPointerType(_class)) {
            _class.pop_back();
        }
        
        m = _ast.getMethodReturnType(name, object.type());
        
        type = m.type;
        
    } else {
        type = _ast.getFunctionReturnType(name);
    }
    
    if (funcall.object != nullptr) {
        name = NameMangler::premangleMethodName(name, m.className);
    }
    
    return (AASTNode *)new AASTFuncall(name, type, params);
    
}

AASTNode * Analyzer::getFuncallParameter(ASTNode * node) {
    
    if (node->nodeType == NodeType::FunCall) {
        
        ASTFunCall * funcall = (ASTFunCall*)node;
        
        /* Recurse if funcall is passed as a parameter to a funcall */
        AASTNode * fcall = analyzeFunCall(*funcall);
        return fcall;
        
    } else if (node->nodeType == NodeType::Variable) {
        
        ASTVariable & variable = *((ASTVariable*)node);
        
        /* Get var name and type */
        parameter var = getVariable(variable);
        
        AASTNode * node = (AASTNode *) new AASTValue(var.value, var.type);
        
        return node;
        
    } else if (node->nodeType == NodeType::Literal) {
        
        ASTLiteral & literal = *((ASTLiteral*)node);
        
        /* Get literal type and value */
        
        parameter lit;
        
        lit.type = literal.type;
        
        /* If literal is a character literal, surround character literal with single quotes ' */
        /* If literal is a char* literal, surround it with quotes " */
        if (literal.type == "char") {
            lit.value = "'" + literal.value + "'";
        } else if (literal.type == syntax::pointerForType("char")) {
            lit.value = "\"" + literal.value + "\"";
        } else {
            lit.value = literal.value;
        }
        
        AASTNode * node = (AASTNode *) new AASTValue(lit.value, lit.type);
        return node;
        
    } else if (node->nodeType == NodeType::MemberAccess) {
        
        ASTMemberAccess & attribute = *((ASTMemberAccess*)node);
        return  ((AASTNode *) new AASTValue(analyzeMemberAccess(attribute)));
        
    }
    
    throw invalid_syntax("Invalid parameter in a function call in function: " + currentFunction());
    
}

AASTOperator * Analyzer::analyzePrint(std::vector<AASTNode *> & parameters) {
    
    
    /* If no parameters are passed print \n */
    if (not parameters.size()) {
        parameters.emplace_back((AASTNode *)new AASTValue("\"\n\"", syntax::pointerForType("char")));
    }
    
    for (AASTNode * node : parameters) {
        
        const std::string & type = node->type();
        
        if (type != "char" and (not syntax::isPointerType(type)) and
            type != "int" and type != "uint" and type != "num") {
            throw invalid_parameter(currentFunction(), "(print)", node->value());
        }
        
    }
    
    return new AASTOperator("print", "", parameters);
    
}

AASTScope * Analyzer::analyzeReturn(std::vector<AASTNode *> & parameters) {
    
    /* Make sure no more than one value is being returned */
    if (parameters.size() > 1) {
        throw invalid_call("(return)", currentFunction(), "Too many parameters.");
    }
    
    /* Check type */
    if (parameters.size()) {
        
        try {
            
            parameters.front() = cast(parameters.front(), _functionType);
            
        } catch (const invalid_cast & e) {
            
            throw type_mismatch("Returning value of invalid type in function " + currentFunction() +
                                " Expected: " + _functionType + " Got: " + parameters.front()->type());
            
        }
        
    } else {
        
        if (_functionType != "void") {
            throw type_mismatch("Returning value of invalid type in function " + currentFunction() +
                                " Expected: " + _functionType + " Got: void");
        }
        
    }
    
    std::vector<AASTNode *> calls;
    
    AASTDeclaration * declaration = nullptr;
    AASTValue * retval = nullptr;
    
    if (parameters.size()) {
        /* Store return value in a temporary variable that won't be destructed */
        declaration = new AASTDeclaration("__RetVal", parameters.front()->type(), parameters.front());
        retval = new AASTValue("__RetVal", declaration->type());
        
        calls.emplace_back(declaration);
    }
    
    /* Call destructors */
    
    for (auto & i : _declarations) {
        
        if (_ast.isClass(i->type) and (not syntax::isPointerType(i->type))  and _ast.hasDestructor(i->type)) {
            
            AASTNode * object = (AASTNode *)new AASTValue(i->name, i->type);
            calls.emplace_back((AASTNode *)getDestructor(object));
            
        }
    }
    
    if (retval != nullptr) {
        calls.emplace_back((AASTNode *)new AASTOperator("return", "void", { retval }));
    } else {
        calls.emplace_back((AASTNode *)new AASTOperator("return", "void", parameters));
    }
    
    return new AASTScope(calls);
}

AASTScope * Analyzer::deleteObject(AASTNode * object) {
    
    
    AASTFuncall * destructor = nullptr;
    AASTFuncall * free = nullptr;
    AASTOperator * setToNull = nullptr;
    
    if (not syntax::isPointerType(object->type())) {
        throw invalid_parameter("Delete must be called on pointer type.");
    }
    
    std::string type = object->type();
    type.pop_back();
    
    if (_ast.isClass(type)) {
        
        if (_ast.hasDestructor(type)) {
            
            destructor = getDestructor(object);
            
        }
    }

    AASTOperator * objectAddress =
        new AASTOperator("&", syntax::pointerForType(type), std::vector<AASTNode *>( {object} ));
    
    free = new AASTFuncall("free", "void", std::vector<AASTNode *>( { objectAddress } ));
    
    if (object->nodeType() == AASTNodeType::Value) {
        AASTValue * null_ptr = new AASTValue("0", syntax::pointerForType("void"));
        setToNull = new AASTOperator("=", "void", std::vector<AASTNode *>( { object , null_ptr } ));
    }
    
    std::vector<AASTNode *> body;
    
    if (destructor != nullptr) {
        body.emplace_back((AASTNode *)destructor);
    }
    if (free != nullptr) {
        body.emplace_back((AASTNode *)free);
    }
    if (setToNull != nullptr) {
        body.emplace_back((AASTNode *)setToNull);
    }
    
    return new AASTScope(body);
    
}

std::vector<AASTFuncall *> Analyzer::destructScopedObjects(std::vector<ASTNode *> scopeNodes) {
    
    /* Search for variable declarations. If any variables are declared in scope,         */
    /* check if they have destructors and call the destructors to delete local variables */
    
    std::vector<parameter> declarations;
    
    for (ASTNode * node : scopeNodes) {
        if (node->nodeType == NodeType::Declaration) {
            
            const ASTDeclaration & d = *((ASTDeclaration*)node);
            const parameter p(d.name, d.type);
            declarations.emplace_back(p);
            
        }
    }
    
    std::vector<AASTFuncall *> destructorCalls;
    
    for (auto & i : declarations) {
        
        if (_ast.isClass(i.type) and (not syntax::isPointerType(i.type))  and _ast.hasDestructor(i.type)) {
            
            AASTNode * object = (AASTNode *)new AASTValue(i.value, i.type);
            destructorCalls.emplace_back(getDestructor(object));
            
        }
    }
    
    return destructorCalls;
    
}

AASTFuncall * Analyzer::getDestructor(AASTNode * object) {
    
    std::string _class = object->type();
    if (syntax::isPointerType(_class)) {
        _class.pop_back();
    }
    
    std::string destructorName = NameMangler::mangleName("destruct", std::vector<std::string>());
    
    method m = _ast.getMethodReturnType(destructorName, _class);
    
    destructorName = NameMangler::premangleMethodName(destructorName, m.className);
    
    if (_class != m.className) {
        if (syntax::isPointerType(object->type())) {
            object = cast(object, syntax::pointerForType(m.className));
        } else {
            object = cast(object, m.className);
        }
    }
    
    std::vector<AASTNode *> objectVector;
    objectVector.emplace_back(object);
    
    AASTNode * objectAddress = (AASTNode *)analyzeOperator("&", objectVector);
    
    return new AASTFuncall(destructorName, m.type, std::vector<AASTNode *>( { objectAddress } ));
    
}


AASTNode * Analyzer::cast(AASTNode * valueToCast, const std::string & type) {
    
    /* No cast needed if types are equal */
    if (valueToCast->type() == type) {
        return valueToCast;
    }
    
    /* If both values are pointers, they can be casted via void*, allow cast */
    else if ( syntax::isPointerType(valueToCast->type()) and syntax::isPointerType(type)) {
        return new AASTCast(valueToCast, type);
    }
    
    /* If both values are numerical types, allow cast */
    else if (expr::isNumericalType(type) and expr::isNumericalType(valueToCast->type())) {
        return new AASTCast(valueToCast, type);
    }
    
    /* Check if classes are related, doesn't matter which one inherits from which */
    /* If one class does inherit from another, cast via void*                     */
    else if (_ast.hasSuperclass(valueToCast->type(), type) or
             _ast.hasSuperclass(type, valueToCast->type())) {
        
        AASTOperator * objectAddress = new AASTOperator("&", type, std::vector<AASTNode *>( { valueToCast } ));
        AASTCast * pointerCast = new AASTCast(objectAddress, syntax::pointerForType(type));
        return new AASTOperator("*", type, std::vector<AASTNode *>( { pointerCast } ));
        
    }
    
    throw invalid_cast(valueToCast->type(), type);
    
}

AASTOperator * Analyzer::analyzeOperator(const std::string & op, std::vector<AASTNode *> & params) {
    
    const bool isParamless  = expr::isParameterlessOperator(op);
    const bool isUnary      = expr::isUnaryOperator(op);
    const bool isBinary     = expr::isBinaryOperator(op);
    
    if (isParamless) {
        
        if (params.size() and not (isUnary or isBinary)) {
            throw invalid_call(op, currentFunction(), " Too many parameters");
        }
        
        if (not params.size()) {
            return expr::parameterless_operator(op);
        }
        
    }
    
    if (isUnary) {
        
        if (not params.size()) {
            throw invalid_call(op, currentFunction(), " Too few parameters");
        } else if (params.size() > 1 and not isBinary) {
            throw invalid_call(op, currentFunction(), " Too many parameters");
        }
        
        if (params.size() == 1) {
            return expr::unaryOperator(params[0], op);
        }
        
    }
    
    if (isBinary) {
        
        if (params.size() < 2) {
            throw invalid_call(op, currentFunction(), " Too few parameters");
        }
        
        return expr::binaryOperator(params, op);
        
    }
    
    throw invalid_operator(op);
    
}

AASTConstruct * Analyzer::analyzeConstruct(ASTConstruct & construct) {
    
    static const std::array<NodeType, 4> allowed_node_types = {
        NodeType::Variable, NodeType::Literal, NodeType::MemberAccess, NodeType::FunCall
    };
    
    if (not contains(allowed_node_types, construct.condition->nodeType)) {
        
        throw invalid_syntax("Error: Invalid condition in construct " +
                             construct.construct + " in function " + currentFunction());
    }
    
    AASTNode * condition = nullptr;
    
    /* Analyze condition if construct isn't else           */
    /* Else doesn't have a condition, everything else does */
    if (construct.construct != "else") {
    
        condition = getFuncallParameter(construct.condition);
        
        if (not (expr::isNumericalType(condition->type()) or syntax::isPointerType(condition->type()))) {
            throw type_mismatch("Error: Conditions must be of numerical or pointer type. Construct: " +
                                construct.construct + " Function: " + currentFunction());
        }
        
    }
    
    /* If construct is elif, expand to else if */
    const std::string cstruct = construct.construct == "elif" ? "else if" : construct.construct;
    
    return new AASTConstruct(cstruct, condition, analyzeScope(construct.childNodes));
    
}

/* If type is to be inferred, inferred type is also stored in ast. */
AASTDeclaration * Analyzer::analyzeDeclaration(ASTDeclaration & declaration) {
    
    std::string decl;
    
    checkIdIsValid(declaration.name);
    
    std::string type = declaration.type;
    AASTNode * value = nullptr;
    
    static const std::array<NodeType, 4> allowed_node_types = {
        NodeType::Variable, NodeType::Literal, NodeType::MemberAccess, NodeType::FunCall
    };
    
    if (declaration.value != nullptr) {

        if (not contains(allowed_node_types, declaration.value->nodeType)) {
            throw invalid_syntax("Error: Invalid value assigned to variable " + declaration.name +
                                 " in function " + currentFunction());
        }
        
        value = getFuncallParameter(declaration.value);
        
        if (type == "var" or type == syntax::pointerForType("var")) {
            type = value->type();
            declaration.type = type;
        }
        
        if (syntax::isPointerType(type) and not syntax::isPointerType(value->type())) {
            value = (AASTNode *)new AASTOperator("&", value->type() + syntax::pointerChar,
                                                 std::vector<AASTNode *>( { value } ));
        }
        
    }
    
    _ast.emplaceVariableIntoScope(parameter(declaration.name, type), declaration.parentScope);
    
    return new AASTDeclaration(declaration.name, type, value);
    
}

AASTValue Analyzer::analyzeMemberAccess(ASTMemberAccess & attribute) {
    
    if (attribute.accessOrder.size() < 1) {
        throw invalid_attribute_access(currentFunction(), "Not enough parameters.");
    }
    
    parameter attr;
    
    AASTNode * baseValue;
    parameter baseVal;
    
    if ((*attribute.accessOrder[0]).nodeType == NodeType::FunCall) {
        
        ASTFunCall & fcall = *((ASTFunCall*)attribute.accessOrder[0]);
        baseValue = analyzeFunCall(fcall);
        baseVal.type = baseValue->type();
        baseVal.value = baseValue->value();
        
    } else {
        
        baseVal.name = ((ASTVariable*)attribute.accessOrder[0])->name;
        baseVal.type = _ast.getVarType(baseVal.name, attribute.parentScope);
        baseValue = new AASTValue(baseVal.name, baseVal.type);
        
    }
    
    if (attribute.accessOrder.size() > 1) {
        
    } else {
        attr.type = baseVal.type;
    }
    
    const bool isPtr = syntax::isPointerType(baseVal.type);
    
    if (isPtr) {
        baseVal.type.pop_back();
        baseVal.name = "(*" + baseVal.name + ")";
    }
    
    std::vector<ASTNode*> accessedVars;
    accessedVars.emplace_back(attribute.accessOrder[0]);
    
    attr.value = baseVal.name + ".";
    
    for (size_t i = 1; i < attribute.accessOrder.size(); ++i) {
        
        std::string & a = ((ASTVariable*)attribute.accessOrder[i])->name;
        accessedVars.emplace_back(attribute.accessOrder[i]);
        
        attr.type = checkAttributesAndReturnType(baseVal, accessedVars);
        
        const bool isMemberPointer = syntax::isPointerType(attr.type);
        
        attr.value += a + (isMemberPointer ? "->" : ".");
        
        
        
    }
    
    /* First item is dereferenced, so if the only item is self, remove pointer char to reflect this */
    if (attribute.accessOrder.size() == 1 and syntax::isPointerType(attr.type)) {
        attr.type.pop_back();
    }
    
    /* Pop back twice if value ends with ->, otherwise pop back once, because value ends with . */
    if (attr.value.back() == '>') {
        attr.value.pop_back();
    }
    attr.value.pop_back();
    
    delete baseValue;
    
    return AASTValue(attr.value, attr.type);
    
}

std::string Analyzer::checkAttributesAndReturnType(parameter & var, std::vector<ASTNode*> & attributes, unsigned int iter) {
    
    /* Probably unnecessary */
    /* if (iter != attributes.size() - 1) {
     checkIsClass(var.type);
     } */
    
    parameter v = var;
    
    if (syntax::isPointerType(var.type)) {
        v.type.pop_back();
        v.value = "(*" + v.value + ")";
    }
    
    checkIsClass(v.type);
    
    std::string & attribute = ((ASTVariable*)attributes[iter])->name;
    
    const _class & cls = _ast.getClass(v.type);
    
    if (not cls.hasVar(attribute)) {
        throw invalid_attribute_access(currentFunction(), "Class " + cls.className + " has no attribute " + attribute);
    }
    
    std::string type = cls.getVarType(attribute);
    
    if (iter == attributes.size() - 1) {
        return type;
    }
    
    if (syntax::isPointerType(type)) {
        type.pop_back();
    }
    
    parameter baseVar(attribute, type);
    
    return checkAttributesAndReturnType(baseVar, attributes, iter + 1);
    
}

void Analyzer::checkIsClass(std::string & className) {
    
    if (not _ast.isClass(className)) {
        throw invalid_attribute_access(currentFunction(), className + " is not a class.");
    }
    
}

AASTOperator * Analyzer::inlineC(std::vector<AASTNode *> & parameters, ASTFunCall & fcall) {
    
    for (ASTNode * p : fcall.parameters) {
        
        if (p->nodeType != NodeType::Literal) {
            throw invalid_syntax("Error: Inline C must receive string literals as parameters. ");
        }
        
        ASTLiteral & lit = *(ASTLiteral*)p;
        
        if (lit.type != syntax::pointerForType("char")) {
            throw invalid_syntax("Error: Inline C must receive string literals as parameters. ");
        }
        
    }
    
    return new AASTOperator("_c", "void", parameters);
    
}

AASTOperator * Analyzer::newObject(const std::string & type) {
    
    if (not _ast.isDataType(type)) {
        throw undefined_class(type);
    }
    
    AASTValue * param = new AASTValue(type, type);
    
    return new AASTOperator("new", syntax::pointerForType(type), std::vector<AASTNode *>( { param } ));
    
}

std::string Analyzer::currentFunction() {
    return "(" + _functionType + " " + _functionName + ")";
}

void Analyzer::analyze() {
    
    kobericMainCheck();
    
    analyzeClasses();
    analyzeGlobalVars();
    analyzeFunctions();
    
}
