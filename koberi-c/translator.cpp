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
        params.back().value = _tokens[i+1].value;
        
    }
    
}

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
        else if (parenCounter == 1) { params.emplace_back(_tokens[iter].value); }
        
    }
    
    if (funName == "toNum" and params.size() == 1) {
        expr = expr::conversionToNum(params[0]);
    }
    
    return expr;
    
}

void Translator::parseFun(unsigned long long funBeginning, unsigned long long funEnd) {
    
    _localVars = std::unordered_map<std::string, std::string>();
    
#if PRINT_FUNCTIONS_TOKENS
    for (unsigned long long i = funcBeginning; i < funcEnd; ++i) {
        print(_tokens[i].value);
    }
#endif
    
    std::string type = _tokens[funBeginning + 1].value;
    std::string name = _tokens[funBeginning + 2].value;
    std::vector<parameter> params;
    
    parseParams(funBeginning + 3, params);
    
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
    
    // TODO: make it work
    // _output << parseSexp(0) << ";\n"; /* Appending a semicolon at the end - semicolons after if () {} don't do anything */
    
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
            << "    sprintf(temp, \"%f\", param);\n"
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
