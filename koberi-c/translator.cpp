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
    
    for (auto & i : params) {
        
        name += "_" + i.type.substr(0, 1);
        
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

std::string Translator::parseSexp(unsigned long long sexpBeginning) {
    
    std::string expr;
    
    std::vector<std::string> params;
    std::string funName = _tokens[sexpBeginning + 1].value;
    
    unsigned long long iter = sexpBeginning + 2; /* sexpBeginning is parenthesis, sexpBeginning + 1 is function name,
                                                    sexpBeginning + 2 is the first param */
    
    for (unsigned int parenCounter = 1; parenCounter != 0; ++iter ) {
    
        /* Recursively parse sexps nested in other sexps */
        /* If parenCounter == 2 so only the first expression is parsed in this function call, 
           expressions nested in nested expressions will be parsed recursively */
        
        if (_tokens[iter] == tokType::openingPar and parenCounter == 2) {
            
            ++parenCounter;
            params.emplace_back(parseSexp(iter));
            
        }
        else if (_tokens[iter] == tokType::openingPar) { ++parenCounter; }
        else if (_tokens[iter] == tokType::closingPar) { --parenCounter; }
        else if (parenCounter == 1) { params.emplace_back(_tokens[iter].value); }
        
    }
    
    return expr;
    
}

void Translator::parseFunc(unsigned long long funcBeginning, unsigned long long funcEnd) {
    
    _localVars = std::unordered_map<std::string, std::string>();
    
#if PRINT_FUNCTIONS_TOKENS
    for (unsigned long long i = funcBeginning; i < funcEnd; ++i) {
        print(_tokens[i].value);
    }
#endif
    
    std::string type = _tokens[funcBeginning + 1].value;
    std::string name = _tokens[funcEnd + 2].value;
    std::vector<parameter> params;
    
    parseParams(funcBeginning + 3, params);
    mangleName(name, params);
    
}

void Translator::varDeclaration(unsigned long long declBeginning, unsigned long long declEnd) {
    
    if (declEnd - declBeginning < 4) {
        
        std::stringstream ss;
        
        for (unsigned long long i = declBeginning; i < declEnd; ++i) {
            ss << _tokens[i].value;
        }
        
        throw invalid_declaration(ss.str());
    
    }
    
    std::string type = _tokens[declBeginning + 2].value;
    std::string name = _tokens[declBeginning + 3].value;
    _globalVars.emplace(name, type);
    
    
}

void Translator::funDeclaration(unsigned long long declBeginning, unsigned long long declEnd) {
    
    std::string type = _tokens[declBeginning + 1].value;
    std::string name = _tokens[declBeginning + 2].value;
    std::vector<parameter> params;
    
    parseParams(declBeginning + 3, params);
    mangleName(name, params);
    _functions.emplace(name, type);
    
    std::stringstream ss;
    ss << type << " " << name << "(";
    
    size_t size = params.size();
    
    for (size_t i = 0; i < size; ++i) {
        
        ss << params[i].type << " " << params[i].value << ((i != size - 1) ? ", " : "");
        
    }
    if (not size) { ss << "void"; }
    ss << ");";
    
    _output << ss.str() << std::endl;
    
#ifdef OUTPUT_FUNCTION_DECLARATION
    print(ss.str());
#endif
    
}

void Translator::parseDeclarations() {
    
    unsigned long long parens = 0;
    unsigned long long declBeginning = 0, declEnd = 0;
    
    for (unsigned long long i = 0; i < _tokens.size(); ++i) {
        
        if (_tokens[i].type == tokType::openingPar) {
            
            ++parens;
            if (parens == 1) { declBeginning = i; }
            
        }
        else if (_tokens[i].type == tokType::closingPar) {
            
            --parens;
            if (not parens) {
                
                declEnd = i;
                if (_tokens[i].value == "global") {
                    varDeclaration(declBeginning, declEnd);
                    i = declEnd;
                    parens = 0;
                }
                else {
                    funDeclaration(declBeginning, declEnd);
                    i = declEnd;
                    parens = 0;
                }
                
            }
            
        } /* Else If */
        
    } /* For */
    
}

void Translator::libs() {
    
    _output << "#include <stdio.h>\n"
            << "#include <stdlib.h>\n"
            << "#include <math.h>\n"
            << "#include <time.h>\n"
            << std::endl;
    
}

void Translator::typedefs() {
    
    _output << "typedef double num;\n"
            << "typedef long long ll;\n"
            << std::endl;
    
}

void Translator::translate() {
    
    libs();
    typedefs();
    
    parseDeclarations();
    
#if 0
    
    unsigned long long parens = 0;
    unsigned long long funcBeginning = 0, funcEnd = 0;
    
    for (int i = 0; i < _tokens.size(); ++i) {
        
        if (_tokens[i].type == tokType::openingPar) {
            
            ++parens;
            if (parens == 1) { funcBeginning = i; }
            
        }
        else if (_tokens[i].type == tokType::closingPar) {
            
            --parens;
            if (not parens) {
                funcEnd = i;
                parseFunc(funcBeginning, funcEnd);
            }
            
        } /* Else If */
        
    } /* For */
#endif
}
