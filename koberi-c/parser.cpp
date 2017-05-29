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
        params.back().name = _tokens[i+1].value;
        
    }
    
}

/* Pretend this method does what is says it does and ignore it  */
/* Seriously                                                    */
/* For your own well-being                                      */

/* Code is being rewritten, this monstrosity will hopefully be removed soon™ */

void Parser::parseSexp(unsigned long long sexpBeginning) {
    
    unsigned long long tokensLen = _tokens.size();
    
    parameter expr;
    
    std::vector<parameter> params;
    std::string funName = _tokens[sexpBeginning + 1].value;
    
    if (_tokens[sexpBeginning + 1].type == tokType::intLit) {
        expr.value = _tokens[sexpBeginning + 1].value;
        expr.type = "int";
        // return expr;
    } else if (_tokens[sexpBeginning + 1].type == tokType::numLit) {
        expr.value = _tokens[sexpBeginning + 1].value;
        expr.type = "num";
        // return expr;
    } else if (_tokens[sexpBeginning + 1].type == tokType::strLit) {
        expr.value = _tokens[sexpBeginning + 1].value;
        expr.type = "str";
        // return expr;
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
            // params.emplace_back(parseSexp(iter));
            
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
    
    if (params.size()) {
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
            // return expr;
        } catch(const std::out_of_range & e) {
            /* Do nothing, just go to next step */
        }
        
        try {
            
            
            expr.value = funName;
            // return expr;
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
    
    // return expr;

}

void Parser::parseSexps(unsigned long long firstSexp) {

    const unsigned long long tokensLen = _tokens.size();
    
    int parenCounter = 0;
    unsigned long long iter = firstSexp;
    
    /* Holds indices of sexps */
    std::vector<unsigned long long> sexps;
    token tok;
    
    do {
        
        /* Prevent the compiler from trying to access outside memory bounds */
        if (iter == tokensLen) {
            throw missing_token(')');
        }
        
        tok = _tokens[iter];
        
        if (tok == tokType::openingPar) {
            ++parenCounter;
        }
        else if (tok == tokType::closingPar) {
            --parenCounter;
        }
        
        if (parenCounter == 1 and tok == tokType::openingPar) {
            sexps.emplace_back(iter);
        }
        
        ++iter;
        
    } while (parenCounter >= 0);
    
    for (const auto sexp : sexps) {
        parseSexp(sexp);
    }
    
}

void Parser::parseFun(unsigned long long funBeginning, unsigned long long funEnd) {
    
    
#if PRINT_FUNCTIONS_TOKENS
    for (unsigned long long i = funcBeginning; i < funcEnd; ++i) {
        print(_tokens[i].value);
    }
#endif
    
    std::string type = _tokens[funBeginning + 1].value;
    
    std::string name = _tokens[funBeginning + 2].value;
    std::vector<parameter> params;
    
    parseParams(funBeginning + 3, params);
    
    for (parameter & p : params) {
        p.name = p.name;
    }
    
    /* Emplace function into ast                                                 */
    /* EmplaceFunction also changes current scope to the newly emplaced function */
    _ast.emplaceFunction(name, type, params);
    
    unsigned long long sexp = 0;
    
    /* funBeginning = (; funBeginning + 1 = data type; funBeginning + 2 = name; funBeginning + 3 = ( */
    /* Find closing paren */
    for (sexp = funBeginning + 3; _tokens[sexp] != tokType::closingPar; ++sexp);
    /* Next position is the opening paren of the first s-expression */
    ++sexp;
    
    parseSexps(sexp);
    
    /* Exit the function scope and enter the global scope */
    _ast.exitScope();
    
    
}

void Parser::varDeclaration(unsigned long long declBeginning, unsigned long long declEnd) {
    
    parameter var = parseVariable(declBeginning);

    _ast.emplaceDeclaration(var.type, var.name);
    
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
        
        if (tok == tokType::openingPar) {
            ++parenCounter;
        }
        else if (tok == tokType::closingPar) {
            --parenCounter;
        }
        
        if (parenCounter == 1 and tok == tokType::openingPar) {
            sexps.emplace_back(iter);
        }
        else if (parenCounter > 1) {
            throw unexpected_token(')');
        }
        
        ++iter;
        
    } while (parenCounter >= 0);
    
    parameter param;
    std::vector<parameter> members;
    for (auto attribute : sexps) {
        
        param = parseVariable(attribute);
        
        /* Check if attribute isn't being redefined twice in one class definition */
        
        for (auto & i : members) {
            if (i.name == param.name) {
                throw redefinition_of_attribute(param.name, className);
            }
        }

        members.emplace_back(param);
    
    }
    
    return members;
    
}

parameter Parser::parseVariable(unsigned long long sexpBeginning) {
    
    parameter param;
    
    param.type = _tokens[sexpBeginning + 1].value;
    param.name = _tokens[sexpBeginning + 2].value;
    
    if (_tokens[sexpBeginning + 3] != tokType::closingPar) {
        
        throw invalid_declaration("Class attributes and global variables can't be initialized with default values. Initialze \
                                  them using a function. ");
    }
    
    return param;
    
}

void Parser::classDefinition(unsigned long long defBeginning, unsigned long long defEnd ) {
    
    std::string name = _tokens[defBeginning + 2].value;
    std::vector<parameter> params;
    
    std::string superclass;
    
    /* Assume class doesn't inherit from anything so fourth token is paren */
    unsigned long long firstDeclaration = defBeginning + 5;
    
    /* 0th token is paren, first is class, second is name, third is opening paren, fourth is closing paren or superclass name */
    /* If 4th token isn't a closing parenthesis, class is most likely inheriting from another superclass                      */
    if (_tokens[defBeginning + 4] != tokType::closingPar) {
        
        /* Increment firstDeclaration by one to reflect the fact that this class inherits from a superclass */
        ++firstDeclaration;
        
        superclass = _tokens[defBeginning + 4].value;
        
    }
    
    if (_tokens[firstDeclaration - 1] != tokType::closingPar) {
        
        throw invalid_syntax("Classes can only inherit from 1 superclass. ");
        
    }
    
    params = parseClassMembers(firstDeclaration, name);
    
    _ast.emplaceClass(name, superclass, params);
    
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
        
        classDefinition(defBeginning, defEnd);
        
    } else if ( _tokens[defBeginning + 3].type == tokType::openingPar ) {
        
        parseFun(defBeginning, defEnd);
    
    } else {
        
        varDeclaration(defBeginning, defEnd);
        
    }
    
}

void Parser::parse() {
    
    parseDefinitions();
    
}
