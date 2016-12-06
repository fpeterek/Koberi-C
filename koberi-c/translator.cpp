//
//  translator.cpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#include "translator.hpp"


Translator::Translator(std::vector<token> & vectorRef) : _tokens(vectorRef) /* Call the reference constructor */ {
    
    _output.open("output.c");
    
    if (not _output) {
        
        throw file_not_opened("output.c");
        
    }
    
}

void Translator::checkType(std::string & type) {
    
    if (not (dataTypes >> type)) {
        throw bad_type("Invalid data type " + type);
    }
    
}

std::string Translator::getType(token & tok) {
    
    std::string type;
    
    if (tok == tokType::intLit) {
        type = "int";
    } else if (tok == tokType::numLit) {
        type = "num";
    } else if (tok == tokType::strLit) {
        type = "str";
    } else {
        
        if (_localVars.find(tok.value) != _localVars.end()) {
            type = _localVars[tok.value];
        }
        else if (_globalVars.find(tok.value) != _localVars.end()) {
            type = _globalVars[tok.value];
        } /* else {
            throw undefined_variable(tok.value);
        } */
    }
    
    return type;
    
}

std::string Translator::getVarType(parameter & param) {
    
    std::string type;
    
    if (_localVars.find(param.value) != _localVars.end()) {
        
        type = _localVars[param.value];
    
    }
    else if (_globalVars.find(param.value) != _localVars.end()) {
    
        type = _globalVars[param.value];
    
    }
    else {
       
        throw undefined_variable(param.value);
    
    }
    
    return type;
    
}

void Translator::mangleName(std::string & name, std::vector<parameter> & params) {
    
    if (name == "main") { return; }
    
    name.insert(0, "_");
    
    for (auto & i : params) {
        
        name += "_" + i.type;
        
    }
    
}

void Translator::parseParams(unsigned long long beginning, std::vector<parameter> & params) {
    
    for (unsigned long long i = beginning + 1; _tokens[i] != tokType::closingPar and _tokens[i+1] != tokType::closingPar; i += 2) {
        
        if (_tokens[i] == tokType::closingPar or _tokens[i+1] == tokType::closingPar) { break; }
        params.emplace_back();
        params.back().type = _tokens[i].value;
        checkType(params.back().type);
        params.back().value = _tokens[i+1].value;
        
    }
    
}

/* Pretend this method does what is says it does and ignore it  */
/* Seriously                                                    */
/* For your own well-being                                      */

parameter Translator::parseSexp(unsigned long long sexpBeginning) {
    
    parameter expr;
    
    std::vector<parameter> params;
    std::string funName = _tokens[sexpBeginning + 1].value;
    
    unsigned long long iter = sexpBeginning + 2; /* sexpBeginning is parenthesis, sexpBeginning + 1 is function name,
                                                    sexpBeginning + 2 is the first param */
    
    for (unsigned int parenCounter = 1; parenCounter != 0; ++iter ) {
    
        /* Recursively parse sexps nested in other sexps */
        /* if (parenCounter == 1 and token == '(') {} so only the first expression is parsed in this function call,
           expressions nested in other expressions will be parsed recursively */
        
        if (_tokens[iter] == tokType::openingPar and parenCounter == 1) {
            
            ++parenCounter;
            params.emplace_back(parseSexp(iter));
            
        }
        else if (_tokens[iter] == tokType::openingPar) { ++parenCounter; }
        else if (_tokens[iter] == tokType::closingPar) { --parenCounter; }
        else if (parenCounter == 1) {
            params.emplace_back( _tokens[iter] == tokType::strLit ? ("\"" + _tokens[iter].value + "\"") : _tokens[iter].value,
                                getType(_tokens[iter]));
        }
        
    }
    
    if (dataTypes >> funName and params.size()) {
        if (params.size() == 1) {
            expr = expr::variableDeclaration(funName, params[0].value);
        }
        else {
            expr = expr::variableDeclaration(funName, params[0].value, params[1].value);
        }
        _localVars.emplace(funName, params[0].value);
    } else if (funName == "toNum" and params.size() == 1 and expr::native_types >> params[0].type) {
        expr = expr::conversionToNum(params[0]);
    } else if (funName == "toInt" and params.size() == 1 and expr::native_types >> params[0].type) {
        expr = expr::conversionToInt(params[0]);
    } else if (expr::unary_operators >> funName and params.size() == 1) {
        expr = expr::unaryOperator(params[0], funName);
        expr.value.insert(0, "\t");
    } else if (expr::binary_operators >> funName and (params[0].type == "int" or params[0].type == "num")) {
        expr = expr::binaryOperator(params, funName);
        expr.value.insert(0, "\t");
    } else if ((funName == "while" or funName == "if" or funName == "elif") and params.size() > 1) {
        expr.value = "\t" + (funName == "elif" ? "else if": funName) + " ( " + params[0].value + " ) {\n";
        expr.type = ".cf";
        for (size_t i = 1; i < params.size(); ++i) {
            expr.value += "\t\t" + params[i].value + ";\n";
        }
        expr.value += "\t}";
    } else if (funName == "else") {
        expr.value = "\telse {\n";
        expr.type = ".cf";
        for (auto & i : params) {
            expr.value += "\t\t" + i.value + ";\n";
        }
        expr.value += "\t}";
    }
    else if (funName == "print") {
        expr = expr::print(params);
    }
    else {
        
        if (params.size()) {
            goto x; /* Eww, but is there a better, readable way? */
        }
        
        /* Check if it's a variable, otherwise call a function */
        
        try {
            expr.type = _localVars.at(funName);
            expr.value = funName;
            return expr;
        } catch(const std::out_of_range & e) {
            /* Do nothing, just go to next step */
        }
        
        try {
            expr.type = _globalVars.at(funName);
            expr.value = funName;
            return expr;
        } catch(const std::out_of_range & e) {
            /* Do nothing, just go to next step */
        }
        
    x:
        
        for (auto & i : params) {
            
            if (i.type == "") {
                i.type = getVarType(i);
            }
            else if (i.type == ".cf") {
                throw invalid_syntax("Invalid syntax. A control flow statement as a function parameter is not allowed");
            }
            
        }
        
        mangleName(funName, params);
        
        try {
            expr.type = _functions.at(funName);
        } catch (const std::exception & e) {
            throw undeclared_function_call("Invalid call to function " + funName);
        }
        
        std::stringstream ss;
        
        ss << "\t" << funName << "(";
        
        size_t size = params.size();
        for (size_t i = 0; i < size; ++i) {
            ss << " " << params[i].value << ((i != size - 1) ? "," : " ");
        }
        ss << ")";
        expr.value = ss.str();
    }
    
    return expr;

}

void Translator::parseSexps(unsigned long long firstSexp) {

    /* If anyone manages to have 2**31 - 1 nested s-expressions, they might as well write their own compiler */
    int parenCounter = 0;
    unsigned long long iter = firstSexp;
    std::vector<unsigned long long> sexps;
    token tok;
    
    do {
        
        tok = _tokens[iter];
        
        if (tok == tokType::openingPar) { ++parenCounter; }
        else if (tok == tokType::closingPar) { --parenCounter; }
        
        if (parenCounter == 1 and tok == tokType::openingPar) {
            sexps.emplace_back(iter); }
        
        ++iter;
        
    } while (parenCounter >= 0);
    
    parameter expr;
    
    for (unsigned long long i : sexps) {
        
        expr = parseSexp(i);
        
        /*  Appending a semicolon at the end unless the expression is of type .cf (control flow)   */
        /*       In the end there might be some trailing semicolons, but that would just create    */
        /*       empty expressions which don't matter, unless they are between an if/else if/else  */
        /*  If you wanna help battle the semicolon inflation, don't comment your Kobeři-C code     */
        
        _output << expr.value << (expr.type != ".cf" ? ";" : "") << std::endl;
    
    }
    
}

void Translator::parseFun(unsigned long long funBeginning, unsigned long long funEnd) {
    
    _localVars = std::unordered_map<std::string, std::string>();
    
#if PRINT_FUNCTIONS_TOKENS
    for (unsigned long long i = funcBeginning; i < funcEnd; ++i) {
        print(_tokens[i].value);
    }
#endif
    
    std::string type = _tokens[funBeginning + 1].value;
    checkType(type);
    
    std::string name = _tokens[funBeginning + 2].value;
    std::vector<parameter> params;
    
    parseParams(funBeginning + 3, params);
    
    for (parameter & p : params) {
        _localVars.emplace(p.value, p.type);
    }
    
    mangleName(name, params); /* The name of main won't be mangled */
    
    if (name == "main") {
        _output << "int main(int argc, const char * argv[]) {\n";
    }
    else {
        
        _output << ((type == "int") ? "ll" : type) << " " << name << "(";
        
        size_t size = params.size();
        for (size_t i = 0; i < size; ++i) {
            _output << (params[i].type == "int" ? "ll" : params[i].type) << " " << params[i].value << ((i != size - 1) ? ", " : "");
        }
        if (not size) { _output << "void"; }
        _output << ") {\n";
            
    }
    
    unsigned long long sexp = 0;
    
    /* funBeginning = (; funBeginning + 1 = data type; funBeginning + 2 = name; funBeginning + 3 = ( */
    /* Find location of first s-expression */
    for (sexp = funBeginning + 3; _tokens[sexp] != tokType::closingPar; ++sexp);
    
    parseSexps(sexp + 1);
    
    _output << "}\n\n";
    
}

void Translator::varDeclaration(unsigned long long declBeginning, unsigned long long declEnd) {
    
    std::stringstream ss;
    
    if (declEnd - declBeginning < 4) { /* 0 for (; 1 for (); 2 for (global); 3 for (global type); 4 for (global type name); 
                                          At least 4 tokens are needed to declare a global variable */
        
        for (unsigned long long i = declBeginning; i < declEnd; ++i) {
            ss << (_tokens[i] != tokType::openingPar ? "" : " ") << _tokens[i].value;
        }
        
        throw invalid_declaration("Invalid variable declaration:" + ss.str());
    
    }
    
    std::string type = _tokens[declBeginning + 2].value;
    checkType(type);
    
    std::string name = _tokens[declBeginning + 3].value;
    
    ss << (type == "int" ? "ll" : type) << " " << name;
    
    if (_tokens[declBeginning + 4] != tokType::closingPar) {
        ss << " = " << _tokens[declBeginning + 4].value;
    }
    
    ss << ";";
    
#ifdef OUTPUT_GLOBAL_VARIABLE_DECLARATION
    print(ss.str());
#endif
    
    _output << ss.str() << std::endl;
    
    _globalVars.emplace(name, type);
    
}

void Translator::funDeclaration(unsigned long long declBeginning, unsigned long long declEnd) {
    
    std::string type = _tokens[declBeginning + 1].value;
    checkType(type);
    
    std::string name = _tokens[declBeginning + 2].value;
    if (name == "main" and type != "int") { throw invalid_declaration("Main must return int. "); }
    std::vector<parameter> params;
    
    parseParams(declBeginning + 3, params);
    mangleName(name, params);
    _functions.emplace(name, type);
    
    std::stringstream ss;
    if (name != "main") {
        ss << (type == "int" ? "ll" : type) << " " << name << "(";
    } else {
        _output << "int main(int argc, const char * argv[]);" << std::endl;
#ifdef OUTPUT_FUNCTION_DECLARATION
        print("int main(int argc, const char * argv[]);");
#endif
        return;
    }
    size_t size = params.size();
    
    for (size_t i = 0; i < size; ++i) {
        
        ss << (params[i].type == "int" ? "ll" : params[i].type) << " " << params[i].value << ((i != size - 1) ? ", " : "");
        
    }
    if (not size) { ss << "void"; }
    ss << ");";
    
    _output << ss.str() << std::endl;
    
#ifdef OUTPUT_FUNCTION_DECLARATION
    print(ss.str());
#endif
    
}

void Translator::parseDeclarations() {
    
    _output << "/* User defined function declarations and global variables. */\n\n";
    
    unsigned long long parens = 0;
    unsigned long long declBeginning = 0, declEnd = 0;
    
    for (unsigned long long i = 0; i < _tokens.size(); ++i) {
        
        if (_tokens[i].type == tokType::openingPar) {
            
            ++parens;
            if (parens == 1) { declBeginning = i; }
            
        }
        else if (_tokens[i].type == tokType::closingPar) {
            
            --parens;
            
        }
        
        if (not parens) {
            
            declEnd = i;
            declaration(declBeginning, declEnd);
            
        }
        
    } /* For */
    
    _output << "\n"; /* Output two newlines after function declarations, mostly for readability */
    
}

void Translator::declaration(unsigned long long declBeginning, unsigned long long declEnd) {
    
    if (_tokens[declBeginning + 1].value == "global") {
        
        varDeclaration(declBeginning, declEnd);
        
    } else {
        
        funDeclaration(declBeginning, declEnd);
        
    }
    
}

void Translator::libs() {
    
    _output << "#include <stdio.h>\n"
            << "#include <stdlib.h>\n"
            << "#include <math.h>\n"
            << "#include <time.h>\n"
            << "\n"
            << std::endl;
    
}

void Translator::typedefs() {
    
    _output << "typedef double num;\n"
            << "typedef long long ll;\n"
            << std::endl;
    
}

void Translator::functions() {
    
    _output << "/* Kobeři-c standard library functions */\n\n"
            << "char * __numToStr(num param) { \n"
            << "    char * temp = (char *) malloc(50);\n"
            << "    sprintf(temp, \"%f\", param);\n"
            << "    return temp;\n"
            << "}\n\n"
            << "char * __intToStr(ll param) {\n"
            << "    char * temp = (char *) malloc(50);\n"
            << "    sprintf(temp, \"%lld\", param);\n"
            << "    return temp;\n"
            << "}\n\n" << std::flush;
    
    
}

void Translator::parseDefinitions() {
    
    _output << "/* User defined function definitions */\n" << std::endl;
    
    unsigned long long parens = 0;
    unsigned long long defBeginning = 0, defEnd = 0;
    
    for (int i = 0; i < _tokens.size(); ++i) {
        
        if (_tokens[i].type == tokType::openingPar) {
            
            ++parens;
            if (parens == 1) { defBeginning = i; }
            
        }
        else if (_tokens[i].type == tokType::closingPar) {
            
            --parens;
            
        }
        
        if (not parens) {
            defEnd = i;
            definition(defBeginning, defEnd);
        }
        
    } /* For */
    
}

void Translator::definition(unsigned long long defBeginning, unsigned long long defEnd) {
    
    if (_tokens[defBeginning + 1].value != "global") {
        parseFun(defBeginning, defEnd);
    }
    
}

void Translator::translate() {
    
    libs();
    typedefs();
    functions();
    
    parseDeclarations();
    parseDefinitions();
    
}
