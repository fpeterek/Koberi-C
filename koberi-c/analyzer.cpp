//
//  Analyzer.cpp
//  koberi-c
//
//  Created by Filip Peterek on 30/10/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#include "analyzer.hpp"

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
        
        AASTClass _class(cls.className, attributes);
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
            
            AASTDeclaration declaration(decl->name, decl->type, nullptr);
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
    
    const std::vector<parameter> & params = function.parameters;
    
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
    
    AASTFunction analyzedFunction(name, function.type, parameters, analyzeScope(function.childNodes));
    
    _aast.emplaceFunction(analyzedFunction);
    
}

AASTScope Analyzer::analyzeScope(std::vector<ASTNode *> scopeNodes) {
    
    std::vector<AASTNode *> body;
    
    /* Translate all body nodes */
    for (ASTNode * node : scopeNodes) {
        body.emplace_back(analyzeFunctionNode(node));
    }

    /* After translating all body nodes, call destructors on scoped objects */
    std::vector<AASTFuncall> destructors = destructScopedObjects(scopeNodes);
    for (AASTFuncall & i : destructors) {
        body.emplace_back(new AASTFuncall(i));
    }
    
    return AASTScope(body);
    
}

AASTNode * Analyzer::analyzeFunctionNode(ASTNode * node) {
    
    AASTNode * functionNode = nullptr;
    
    if (node->nodeType == NodeType::FunCall) {
        
        ASTFunCall * funcall = (ASTFunCall*)node;
        functionNode = (AASTNode *) new AASTFuncall(analyzeFunCall(*funcall));
        
    } else if (node->nodeType == NodeType::Construct) {
        
        ASTConstruct * c = (ASTConstruct*)node;
        functionNode = (AASTConstruct *) new AASTConstruct(analyzeConstruct(*c));
        
    } else if (node->nodeType == NodeType::Declaration) {
        
        ASTDeclaration * d = (ASTDeclaration*)node;
        functionNode = (AASTDeclaration *) new AASTDeclaration(analyzeDeclaration(*d));
        
    } else {
        
        throw invalid_statement(_functionName);
        
    }
    
    return functionNode;
    
}

AASTNode * Analyzer::analyzeFunCall(ASTFunCall & funcall) {
    
    parameter functionCall;
    std::string name = funcall.function;
    
    /* Only used if function is a member function   */
    /* It needs to be declared at this scope though */
    method m;
    
    /* Casting and new need to be handled separately, because it contains class names */
    if (name == "cast" and funcall.parameters.size() == 2 and funcall.object == nullptr) {
        
        if (funcall.parameters[1]->nodeType == NodeType::Variable and
            _ast.isDataType(((ASTVariable*)funcall.parameters[1])->name)) {
            
            AASTNode * p = getFuncallParameter(funcall.parameters[0]);
            parameter param = parameter(p->type(), p->type());
            std::string type = ((ASTVariable*)funcall.parameters[1])->name;
            
            return cast(param, type);
            
        }
        
    }
    
    if (name == "new" and funcall.parameters.size() == 1 and funcall.object == nullptr) {
        std::string type = ((ASTVariable*)funcall.parameters[0])->name;
        return (AASTNode *)new AASTOperator(newObject(type));
    }
    
    std::vector<parameter> params;
    /* Don't deref char* if statement is a print statement */ {
        const bool derefCharPtr = not (name == "print" and funcall.object == nullptr);
        
        for (auto & param : funcall.parameters) {
            params.emplace_back(getFuncallParameter(param, derefCharPtr));
        }
    }
    
    for (auto & param : params) {
        if (param.type == "") {
            throw invalid_parameter(_functionName, funcall.function, param.value);
        }
    }
    
    if (name == "delete" and funcall.object == nullptr and params.size() == 1) {
        const bool paramIsFuncall = funcall.parameters.size() and funcall.parameters[0]->nodeType == NodeType::FunCall;
        return deleteObject(params[0], paramIsFuncall);
    }
    if (expr::isOperator(name) and funcall.object == nullptr) {
        return translateOperator(name, params);
    }
    if (name == "print" and funcall.object == nullptr) {
        return translatePrint(params);
    }
    if (name == "_c" and funcall.object == nullptr) {
        return inlineC(params);
    }
    
    /* Mangle name and get return type, if function doesn't exist, and exception should be thrown */
    /* If function exists, create valid C function call from provided parameters                  */
    name = NameMangler::mangleName(name, params);
    
    /* Translate parameters first */
    functionCall.value = "(";
    
    if (funcall.object != nullptr) {
        
        parameter object = translateMemberAccess(*funcall.object);
        
        try {
            m = _ast.getMethodReturnType(name, object.type);
        } catch (const undeclared_function_call & e) {
            throw undeclared_function_call(funcall.function, object.type);
        }
        
        /* If method is a member of objects superclass, cast object to that superclass */
        /* Otherwise just access the objects address                                   */
        std::string castedObject = "&(" + object.value + ")";
        if (m.className != object.type) {
            castedObject = "((" + m.className + " *)((void *) " + castedObject + "))";
        }
        functionCall.value += castedObject + (params.size() ? ", " : "");
        functionCall.type = m.type;
        
        
    } else {
        functionCall.type = _ast.getFunctionReturnType(name);
    }
    
    for (size_t i = 0; i < params.size(); ++i) {
        functionCall.value += params[i].value + (i == params.size() - 1 ? "" : ", ");
    }
    
    functionCall.value += ")";
    
    /* After translating the arguments, prepend mangled name in front of them */
    if (funcall.object != nullptr) {
        name = NameMangler::premangleMethodName(name, m.className);
    }
    
    functionCall.value = name + functionCall.value;
    
    return functionCall;
    
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
    
    throw invalid_syntax("Invalid parameter in a function call in function: " + _functionName);
    
}

std::vector<AASTFuncall> Analyzer::destructScopedObjects(std::vector<ASTNode *> scopeNodes) {
    
    /* Search for variable declarations. If any variables are declared in scope,         */
    /* check if they have destructors and call the destructors to delete local variables */
    
    std::vector<parameter> declarations;
    
    for (ASTNode * node : scopeNodes) {
        if (node->nodeType == NodeType::Declaration) {
            
            const ASTDeclaration & d = *((ASTDeclaration*)node);
            parameter p(d.name, d.type);
            declarations.emplace_back(p);
            
        }
    }
    
    std::vector<AASTFuncall> destructorCalls;
    
    for (auto & i : declarations) {
        
        if (_ast.isClass(i.type) and (not isPointer(i.type))  and _ast.hasDestructor(i.type)) {
            
            method m = _ast.getMethodReturnType("destruct", i.type);
            
            std::string castedObject = i.name;
            if (i.type != m.className) {
                castedObject = "((" + m.className + "*)(void*)(&" + castedObject + "))";
            }
            
            std::string destructorName = NameMangler::premangleMethodName("destruct", m.className);
            
            destructorCalls.emplace_back(destructorName, m.type, std::vector<AASTNode *>());
            
        }
    }
    
    return destructorCalls;
    
}

void Analyzer::analyze() {
    
    kobericMainCheck();
    
    analyzeClasses();
    analyzeGlobalVars();
    analyzeFunctions();
    
}
