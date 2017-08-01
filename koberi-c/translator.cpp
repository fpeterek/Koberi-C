//
//  translator.cpp
//  koberi-c
//
//  Created by Filip Peterek on 22/07/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#include "translator.hpp"

Translator::Translator(TraversableAbstractSyntaxTree & ast) : _ast(ast) {
    
}

void Translator::typedefs() {
    
    _output << "\n" << "/* Typedefs */" << "\n";
    
    _output << "\n" << "typedef double num;" << "\n";
    _output << "typedef long long ll;" << "\n\n";
    
}

void Translator::translateClasses() {
    
    const std::unordered_map<std::string, _class> & classes = _ast.getClasses();
    const std::vector<std::string> & classOrder = _ast.getClassOrder();
    
    _output << "\n" << "/* Classes */" << "\n";
    
    for (const auto & c : classOrder) {
        
        const _class & cls = classes.at(c);
        
        _output << "\n" << "typedef struct " << cls.className << "{\n";
        
        for (const auto & attr : cls.attributes) {
            _output << INDENT << (attr.type == "int" ? "ll" : attr.type) << " " << attr.name << ";\n";
        }
        
        _output << "} " << cls.className << ";\n" << std::endl;
        
    }
    
}

void Translator::translateGlobalVars() {
    
    const size_t nodeCount = _ast.getNodeCount();
    
    _output << "\n" << "/* Global Variables */" << "\n" << "\n";
    
    for (size_t i = 0; i < nodeCount; ++i) {
        
        ASTNode * node = _ast.getNodePtr(i);
        
        if (node->nodeType == NodeType::Declaration) {
            
            ASTDeclaration * decl = (ASTDeclaration*)node;
            _output << (decl->type == "int" ? "ll" : decl->type) << " " << decl->name << ";\n";
            
        }
        
    }
    
    std::endl(_output);
    
}

void Translator::forwardFunctionDeclarations() {
    
    const size_t nodeCount = _ast.getNodeCount();
    
    _output << "\n" << "/* Function Declarations */" << "\n" << "\n";
    
    for (size_t i = 0; i < nodeCount; ++i) {
        
        ASTNode * node = _ast.getNodePtr(i);
        
        if (node->nodeType == NodeType::Function) {
            
            ASTFunction * fun = (ASTFunction*)node;
            std::string mangledName = NameMangler::mangleName(fun->name, fun->parameters);
            
            _output << (fun->type == "int" ? "ll" : fun->type) << " " << mangledName << "(";
            
            if (fun->parameters.size()) {
                
                for (size_t iter = 0; iter < fun->parameters.size(); ++iter) {
                    
                    const parameter & param = fun->parameters[iter];
                    _output << (param.type == "int" ? "ll" : param.type) << " " << param.name
                            << (iter == fun->parameters.size() - 1 ? "" : ", ");
                    
                }
                
            }
            else {
                _output << "void";
            }
            
            _output << ");\n";
            
        }
        
    }
    
    std::endl(_output);
    
}

void Translator::translateFunctions() {
    
    ASTNode * node;
    
    for (size_t i = 0; i < _ast.getNodeCount(); ++i) {
        
        node = _ast.getNodePtr(i);
        
        if (node->nodeType == NodeType::Function) {
            
            ASTFunction & function = *((ASTFunction*)node);
            translateFunction(function);
            
        }
        
    }
    
}

void Translator::main() {
    
    _output << "\n" << "/* C Main Function */" << "\n" << "\n";
    
    _output << "int main(int argc, const char * argv[]) {" << "\n";
    _output << INDENT << "return " << NameMangler::mangleName("main", std::vector<parameter>()) << "();" << "\n";
    _output << "}" << std::endl;
    
}

void Translator::setOutputFile(const std::string & filename) {
    
    _output = std::ofstream(filename);
    
    if (not _output) {
        throw file_not_created(filename);
    }
    
}

void Translator::translate() {
    
#ifdef OUTPUT_GLOBAL_SCOPE_NODES
    test();
#endif
    
    kobericMainCheck();
    
    typedefs();
    translateClasses();
    translateGlobalVars();
    forwardFunctionDeclarations();
    translateFunctions();
    
    main();
    
}

void Translator::kobericMainCheck() {
    
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

void Translator::translateFunction(ASTFunction & function) {
    
    _functionName = function.name;
    
    const std::vector<parameter> & params = function.parameters;
    
    _output << "\n" << (function.type == "int" ? "ll" : function.type) << " "
            << NameMangler::mangleName(function.name, params) << "(";
    
    for (size_t i = 0; i < params.size(); ++i) {
        
        _output << (params[i].type == "int" ? "ll" : params[i].type) << " " << params[i].name
                << (i == params.size() - 1 ? "" : ", ");
        
    }
    
    _output << ") {" << "\n";
    ++_indentLevel;
    
    for (ASTNode * node : function.childNodes) {
        
        if (node->nodeType == NodeType::FunCall) {
            
            ASTFunCall * funcall = (ASTFunCall*)node;
            Translator::translateFunCall(*funcall);
            
        } else if (node->nodeType == NodeType::Construct) {
            
            ASTConstruct * construct = (ASTConstruct*)node;
            Translator::translateConstruct(*construct);
            
        } else if (node->nodeType == NodeType::Declaration) {
            
            ASTDeclaration * declaration = (ASTDeclaration*)node;
            std::string declAsStr = Translator::translateDeclaration(*declaration);
            
            indent();
            _output << declAsStr << "\n";
        
        
        } else {
            
            throw invalid_statement(_functionName);
            
        }
        
    }
    
    _output << "}" << "\n" << std::endl;
    --_indentLevel;
    
}

parameter Translator::translateFunCall(ASTFunCall & funcall) {
    
    parameter functionCall;
    
    std::vector<parameter> params;
    
    for (auto param : funcall.parameters) {
        
        if (param->nodeType == NodeType::FunCall) {
            
            ASTFunCall * funcall = (ASTFunCall*)param;
            
            /* Recurse if funcall is passed as a parameter to a funcall */
            parameter fcall = translateFunCall(*funcall);
            
            params.emplace_back(fcall);
            
        } else if (param->nodeType == NodeType::Variable) {
            
            ASTVariable & variable = *((ASTVariable*)param);
            
            /* Get var name and type */
            parameter var = getVariable(variable);
            
            params.emplace_back(var);
            
        } else if (param->nodeType == NodeType::Literal) {
            
            ASTLiteral & literal = *((ASTLiteral*)param);
            
            /* Get literal type and value */
            
            parameter lit;
            
            lit.type = literal.type;
            lit.value = literal.value;
            
            params.emplace_back(lit);
            
        }
        
    }
    
    /* Mangle name and get return type, if function doesn't exist, and exception should be thrown */
    /* If function exists, create valid C function call from provided parameters                  */
    std::string name = NameMangler::mangleName(funcall.function, params);
    
    functionCall.type = _ast.getFunctionReturnType(functionCall.name);
    
    functionCall.value = name + "(";
    
    for (size_t i = 0; i < params.size(); ++i) {
        
        functionCall.value += params[i].value + (i == params.size() - 1 ? "" : ", ");
        
    }
    
    functionCall.value += ")";
    
    // std::cout << "Translate fun call: " << funcall.function << std::endl;
    
    return functionCall;
    
}

void Translator::translateConstruct(ASTConstruct & construct) {
    
}

std::string Translator::translateDeclaration(ASTDeclaration & declaration) {
    
    std::string decl;
    
    decl = (declaration.type == "int" ? "ll" : declaration.type) + " " + declaration.name;
    
    if (declaration.value != nullptr) {
        
        if (declaration.value->nodeType == NodeType::FunCall) {
            
            ASTFunCall * funcall = (ASTFunCall*)declaration.value;
            
            parameter fcall = translateFunCall(*funcall);
            
            if (declaration.type != fcall.type) {
                throw type_mismatch("Error: Type mismatch in declaration of variable ("
                                    + declaration.type + " " + declaration.name + "). Expected: "
                                    + declaration.type +
                                    " Got: " + fcall.type);
            }
            
            decl += " = " + fcall.value;
            
        } else if (declaration.value->nodeType == NodeType::Variable) {
            
            ASTVariable & variable = *((ASTVariable*)declaration.value);
            
            parameter var = getVariable(variable);
            
            if (declaration.type != var.type) {
                throw type_mismatch("Error: Type mismatch in declaration of variable ("
                                    + declaration.type + " " + declaration.name + "). Expected: "
                                    + declaration.type +
                                    " Got: " + var.type);
            }
            
            decl += " = " + var.name;
            
        } else if (declaration.value->nodeType == NodeType::Literal) {
            
            ASTLiteral & literal = *((ASTLiteral*)declaration.value);
            
            if (declaration.type != literal.type) {
                throw type_mismatch("Error: Type mismatch in declaration of variable ("
                                    + declaration.type + " " + declaration.name + "). Expected: "
                                    + declaration.type +
                                    " Got: " + literal.type);
            }
            
            decl += " = " + literal.value;
            
        }
        
    }
    
    decl += ";";
    
    return decl;
    
}

parameter Translator::getVariable(ASTVariable & variable) {
    
    parameter var;
    
    var.name = variable.name;
    var.type = _ast.getVarType(variable.name, variable.parentScope);
    
    return var;
    
}


void Translator::indent() {
    
    for (unsigned short i = 0; i < _indentLevel; ++i) {
        _output << INDENT;
    }
    
}

void Translator::test() {
    
    size_t nodeCount = _ast.getNodeCount();
    
    for (size_t i = 0; i < nodeCount; ++i) {
        
        ASTNode * node = _ast.getNodePtr(i);
        
        switch (node -> nodeType) {
                
            case NodeType::Variable: {
                ASTVariable * var = (ASTVariable*)node;
                std::cout << "Variable: " << var->name << std::endl;
                break;
            }
                
            case NodeType::Literal: {
                ASTLiteral * lit = (ASTLiteral*)node;
                std::cout << "Literal: " << lit->type << ": " << lit->value << std::endl;
                break;
            }
                
            case NodeType::Scope: {
                ASTScope * scope = (ASTScope*)node;
                std::cout << "Scope: nodes: " << scope->childNodes.size() << "vars: " << scope->vars.size() << std::endl;
                break;
            }
                
            case NodeType::FunCall: {
                ASTFunCall * funcall = (ASTFunCall*)node;
                std::cout << "Funcall: " << funcall->function << std::endl;
                break;
            }
                
            case NodeType::Declaration: {
                ASTDeclaration * decl = (ASTDeclaration*)node;
                std::cout << "Declaration: " << decl->type << " " << decl->name << std::endl;
                break;
            }
                
            case NodeType::Function: {
                ASTFunction * fun = (ASTFunction*)node;
                std::cout << "Function: " << fun->type << " " << fun->name << "(";
                for (auto & i : fun->parameters) {
                    std::cout << i.type << " " << i.name << ",";
                }
                std::cout << ")" << std::endl;
                break;
            }
                
            case NodeType::Construct: {
                ASTConstruct * con = (ASTConstruct*)node;
                std::cout << "Construct: " << con->construct << std::endl;
                break;
            }
                
            case NodeType::Attribute: {
                std::cout << "Attribute access: " << "too lazy" << std::endl;
                break;
            }
                
            default:
                
                break;
                
        }
        
    }
    
}
