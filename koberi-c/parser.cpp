//
//  parser.cpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#include "parser.hpp"


Parser::Parser(std::vector<token> & vectorRef, TraversableAbstractSyntaxTree & ast):
        _tokens(vectorRef),
        _ast(ast) /* Call reference constructors */ {
            
}

void Parser::checkType(std::string & type) {
    
    if (not (dataTypes >> type)) {
        throw bad_type("Invalid data type " + type);
    }
    
}

std::string Parser::getType(token & tok) {
    
    std::string type;
    
    if (tok == tokType::intLit) {
        type = "int";
    } else if (tok == tokType::numLit) {
        type = "num";
    } else if (tok == tokType::strLit) {
        type = "str";
    }
    
    return type;
    
}

void Parser::parseParams(unsigned long long beginning, std::vector<parameter> & params) {
    
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

/* Code is being rewritten, this monstrosity will hopefully be removed soon™ */

parameter Parser::parseSexp(unsigned long long sexpBeginning) {
    
    unsigned long long tokensLen = _tokens.size();
    
    parameter expr;
    
    std::vector<parameter> params;
    std::string funName = _tokens[sexpBeginning + 1].value;
    
    if (_tokens[sexpBeginning + 1].type == tokType::intLit) {
        expr.value = _tokens[sexpBeginning + 1].value;
        expr.type = "int";
        return expr;
    } else if (_tokens[sexpBeginning + 1].type == tokType::numLit) {
        expr.value = _tokens[sexpBeginning + 1].value;
        expr.type = "num";
        return expr;
    } else if (_tokens[sexpBeginning + 1].type == tokType::strLit) {
        expr.value = _tokens[sexpBeginning + 1].value;
        expr.type = "str";
        return expr;
    }
    
    unsigned long long iter = sexpBeginning + 2; /* sexpBeginning is parenthesis, sexpBeginning + 1 is function name,
                                                    sexpBeginning + 2 is the first param */
    
    for (unsigned int parenCounter = 1; parenCounter != 0; ++iter ) {
    
        
        /* Prevent the compiler from trying to access outside memory bounds */
        if (iter == tokensLen) {
            throw missing_token(')');
        }
        
        /* Recursively parse sexps nested in other sexps */
        /* if (parenCounter == 1 and token == '(') {} so only the first expression is parsed in this function call,
           expressions nested in other expressions will be parsed recursively */
        
        if (_tokens[iter] == tokType::openingPar and parenCounter == 1) {
            
            ++parenCounter;
            params.emplace_back(parseSexp(iter));
            
        } else if (_tokens[iter] == tokType::openingBra) {
            

            
            while (_tokens[iter] != tokType::closingBra) {
                ++iter;
            }
            
            
        }
        else if (_tokens[iter] == tokType::closingBra) {
            throw unexpected_token(']');
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

        
    } else if (funName == "toNum" and params.size() == 1 and expr::native_types >> params[0].type) {
        expr = expr::conversionToNum(params[0]);
    } else if (funName == "toInt" and params.size() == 1 and expr::native_types >> params[0].type) {
        expr = expr::conversionToInt(params[0]);
    } else if (expr::unary_operators >> funName and params.size() == 1) {
        expr = expr::unaryOperator(params[0], funName);
        expr.value.insert(0, "\t");
    } else if (expr::binary_operators >> funName /*and (params[0].type == "int" or params[0].type == "num")*/) {
        expr = expr::binaryOperator(params, funName);
        expr.value.insert(0, "\t");
    } else if (expr::parameterless_operators >> funName and not params.size()) {
        expr = expr::parameterless_operator(funName);
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
    } else if (funName == "c") {
        expr = expr::inlineC(params);
    }
    else {
        
        if (params.size()) {
            goto x; /* Eww, but is there a better, readable way? */
        }
        
        /* Check if it's a variable, otherwise call a function */
        
        try {
            
            
            expr.value = funName;
            return expr;
        } catch(const std::out_of_range & e) {
            /* Do nothing, just go to next step */
        }
        
        try {
            
            
            expr.value = funName;
            return expr;
        } catch(const std::out_of_range & e) {
            /* Do nothing, just go to next step */
        }
        
    x:
        
        for (auto & i : params) {
            
            if (i.type == "") {
                
                
            }
            else if (i.type == ".cf") {
                throw invalid_syntax("Invalid syntax. A control flow statement as a function parameter is not allowed");
            }
            
        }
        
        
        try {
            
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

void Parser::parseSexps(unsigned long long firstSexp, std::function<parameter(Parser*, unsigned long long)> & fun) {

    const unsigned long long tokensLen = _tokens.size();
    
    /* If anyone manages to have 2**31 - 1 nested s-expressions, they are doing something wrong */
    int parenCounter = 0;
    unsigned long long iter = firstSexp;
    std::vector<unsigned long long> sexps;
    token tok;
    
    do {
        
        /* Prevent the compiler from trying to access outside memory bounds */
        if (iter == tokensLen) {
            throw missing_token(')');
        }
        
        tok = _tokens[iter];
        
        if (tok == tokType::openingPar) { ++parenCounter; }
        else if (tok == tokType::closingPar) { --parenCounter; }
        
        if (parenCounter == 1 and tok == tokType::openingPar) {
            sexps.emplace_back(iter); }
        
        ++iter;
        
    } while (parenCounter >= 0);
    
    parameter expr;
    
}

void Parser::parseFun(unsigned long long funBeginning, unsigned long long funEnd) {
    
    
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
        
    }
    
    unsigned long long sexp = 0;
    
    /* funBeginning = (; funBeginning + 1 = data type; funBeginning + 2 = name; funBeginning + 3 = ( */
    /* Find location of first s-expression */
    for (sexp = funBeginning + 3; _tokens[sexp] != tokType::closingPar; ++sexp);
    
    std::function<parameter(Parser*, unsigned long long)> fun = &Parser::parseSexp;
    
    parseSexps(sexp + 1, fun);
    
    
}

void Parser::varDeclaration(unsigned long long declBeginning, unsigned long long declEnd) {
    
    std::stringstream ss;
    
    if (declEnd - declBeginning < 3) { /* 0 for (; 1 for (); 2 for (type); 3 for (type name);
                                          At least 3 tokens are needed to declare a global variable */
        
        for (unsigned long long i = declBeginning; i < declEnd; ++i) {
            ss << (_tokens[i] != tokType::openingPar ? "" : " ") << _tokens[i].value;
        }
        
        throw invalid_declaration("Invalid variable declaration:" + ss.str());
    
    }
    
    std::string type = _tokens[declBeginning + 1].value;
    checkType(type);
    
    std::string name = _tokens[declBeginning + 2].value;
    
    ss << (type == "int" ? "ll" : type) << " " << name;
    
    if (_tokens[declBeginning + 3] != tokType::closingPar) {
        ss << " = " << _tokens[declBeginning + 3].value;
    }
    
    ss << ";";
    
#ifdef OUTPUT_GLOBAL_VARIABLE_DECLARATION
    print(ss.str());
#endif
    

}

/*
 
 (class name (superclass)
     (int variable)
     (num variable2))
 
*/

std::vector<parameter> Parser::parseClassMembers(unsigned long long firstSexp, std::string & className) {
    
    const unsigned long long tokensLen = _tokens.size();
    
    int parenCounter = 0;
    unsigned long long iter = firstSexp;
    std::vector<unsigned long long> sexps;
    token tok;
    
    do {
        
        /* Prevent the compiler from trying to access outside memory bounds */
        if (iter == tokensLen) {
            throw missing_token(')');
        }
        
        tok = _tokens[iter];
        
        if (tok == tokType::openingPar) { ++parenCounter; }
        else if (tok == tokType::closingPar) { --parenCounter; }
        
        if (parenCounter == 1 and tok == tokType::openingPar) {
            sexps.emplace_back(iter); }
        
        ++iter;
        
    } while (parenCounter >= 0);
    
    parameter param;
    std::vector<parameter> members;
    for (auto attribute : sexps) {
        
        param = parseClassAttribute(attribute);
        
        bool hasAttribute = false;
        for (auto & i : members) {
            if (i.value == param.value) {
                hasAttribute = true;
            }
        }
        
        if (hasAttribute) {
            throw redefinition_of_attribute(param.value, className);
        }
        members.emplace_back(param);
    
    }
    
    return members;
    
}

parameter Parser::parseClassAttribute(unsigned long long sexpBeginning) {
    
    parameter param;
    
    param.type = _tokens[sexpBeginning + 1].value;
    param.value = _tokens[sexpBeginning + 2].value;
    
    return param;
    
}

void Parser::classDefinition(unsigned long long declBeginning, unsigned long long declEnd ) {
    
    std::string name = _tokens[declBeginning + 2].value;
    std::vector<parameter> params;
    
    /* Assume class doesn't inherit from anything so fourth token is paren */
    unsigned long long firstDeclaration = declBeginning + 5;
    
    /* 0th token is paren, first is class, second is name, third is opening paren, fourth is closing paren or superclass name */
    if (_tokens[declBeginning + 4] != tokType::closingPar) {
        
        /* Increment firstDeclaration by one to reflect the fact that this class inherits from a superclass */
        ++firstDeclaration;
        
        std::string superclass = _tokens[declBeginning + 4].value;
        
        
    }
    
    if (_tokens[firstDeclaration - 1] != tokType::closingPar) {
        
        throw invalid_syntax("Classes can only inherit from 1 superclass. ");
        
    }
    
    {
        const std::vector<parameter> temp = parseClassMembers(firstDeclaration, name);
        
        for (auto & i : temp) {
            
            /* Check if class already has such attribute to prevent redefinitions */
            bool hasAttribute = false;
            for (const parameter & v : params) {
                
                if (v.value == i.value) {
                    hasAttribute = true;
                }
                
            }
            
            if (hasAttribute) {
                throw redefinition_of_attribute(i.value, name);
            }
            params.emplace_back(i);
            
            
        }
        
    }
    
    for (auto & i : params) {
        std::string & type = i.type, name = i.value;
        expr::variableDeclaration(type, name);
    }
    
    _class c;
    c.attributes = params;
    
    dataTypes.emplace_back(name);
    
}

/* --- Definition example ---
 
;;; Function
(void x ()
    (if (1)
        (print 1 " evaluates to true")))
 
;;; Class
(class c (base_class)
    (int x)
    (num y))
 
;;; Global variable
(int var1)             ; Allowed
(int var2 10)          ; Allowed
(int var3 (+ 10 10))   ; Not Allowed, global variables must be initialized with a literal or not at all
(int var4 ())          ; Function Definition
(int var5 (int param)) ; Function Definition
 
*/

void Parser::parseDefinitions() {
    
    long long parens = 0;
    unsigned long long definitionBeginning = 0, definitionEnd = 0;
    
    for (int i = 0; i < _tokens.size(); ++i) {
        
        if (_tokens[i].type == tokType::openingPar) {
            
            ++parens;
            if (parens == 1) { definitionBeginning = i; }
            
        }
        else if (_tokens[i].type == tokType::closingPar) {
            
            --parens;
            
        }
        
        if (not parens) {
            definitionEnd = i;
            definition(definitionBeginning, definitionEnd);
        }
        
    } /* For */
    
    if (parens > 0) {
        throw missing_token(')');
    }
    if (parens < 0) {
        throw unexpected_token(')');
    }
    
}

void Parser::definition(unsigned long long defBeginning, unsigned long long defEnd) {
    
    /* Nothing to do here, not now, anyway */
    if (_tokens[defBeginning + 1].value == "class") {
        
        return;
        
    } else if ( _tokens[defBeginning + 3].type == tokType::openingPar ) {
        
        parseFun(defBeginning, defEnd);
    
    }
    
}

void Parser::parse() {
    
    parseDefinitions();
    
}
