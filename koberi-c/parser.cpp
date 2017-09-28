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

bool Parser::isLiteral(unsigned long long tokenIndex) {
    
    tokType t = _tokens[tokenIndex].type;
    
    bool result = t == tokType::strLit or t == tokType::intLit or t == tokType::numLit;
    
    return result;
    
}

ASTLiteral Parser::createLiteral(unsigned long long literalIndex) {
    
    if (_tokens[literalIndex] == tokType::strLit) {
        
        return ASTLiteral("str", _tokens[literalIndex].value);
        
    } else if (_tokens[literalIndex] == tokType::intLit) {
        
        return ASTLiteral("int", _tokens[literalIndex].value);
        
    } else if (_tokens[literalIndex] == tokType::numLit) {
        
        return ASTLiteral("num", _tokens[literalIndex].value);
        
    }
    
    throw compiler_error("Compiler error: Attempting to create a literal from a non-literal token");
    
}

void Parser::parseParams(unsigned long long beginning, std::vector<parameter> & params) {
    
    for (unsigned long long i = beginning + 1; _tokens[i] != tokType::closingPar and _tokens[i+1] != tokType::closingPar; i += 2) {
        
        if (_tokens[i] == tokType::closingPar or _tokens[i+1] == tokType::closingPar) { break; }
        params.emplace_back();
        
        /* Check if parameter type is a valid type */
        if (not _ast.isDataType(_tokens[i].value) ) {
            throw bad_type("Invalid data type: " + _tokens[i].value);
        }
        
        params.back().type = _tokens[i].value;
        params.back().name = _tokens[i+1].value;
        
    }
    
}


void Parser::parseSexp(unsigned long long sexpBeginning) {
    
    // unsigned long long tokensLen = _tokens.size();
    
    const unsigned long long sexpEnd = findSexpEnd(sexpBeginning);
    
    /* If first token is data type, sexp is a variable declaration               */
    /* Functions can't be defined inside functions (unless you compile with GCC) */
    if ( _ast.isDataType(_tokens[sexpBeginning + 1].value) ) {
        
        localVarDeclaration(sexpBeginning, sexpEnd);
    
    }
    else if (expr::isConstruct(_tokens[sexpBeginning + 1].value)) {
        
        parseConstruct(sexpBeginning, sexpEnd);
        
    }
    /* If sexp is neither var declaration, nor construct, it's either a function call */
    /* or an operator                                                                 */
    /* Both function calls and operators are the same thing at this point             */
    /* They will be treated differently during translation, but they are one and      */
    /* the same to the parser/AST during parsing                                      */
    else {
        
        ASTFunCall fcall = parseFunCall(sexpBeginning, sexpEnd);
        
        _ast.emplaceFunCall(fcall);
        
    }

}

void Parser::localVarDeclaration(unsigned long long declBeginning, unsigned long long declEnd) {
    
    const std::string type = _tokens[declBeginning + 1].value;
    
    if ( not _ast.isDataType(type) ) {
        throw invalid_declaration("Unknown data type " + type);
    }
    
    const std::string name = _tokens[declBeginning + 2].value;
    
    
    ASTNode * node = nullptr;
    
    /* If token is opening par, parameter is a funcall */
    if (_tokens[declBeginning + 3] == tokType::openingPar) {
        
        unsigned long long funcallEnd = findSexpEnd(declBeginning + 3);
        ASTFunCall funcall = parseFunCall(declBeginning + 3, funcallEnd);
        
        node = new ASTFunCall(funcall);
        
    } else if (_tokens[declBeginning + 3] == tokType::closingPar) {
        
        node = nullptr;
        
    }
    
    else if (_tokens[declBeginning + 3] == tokType::id) {
        
        node = new ASTVariable( _tokens[declBeginning + 3].value, _ast.getCurrentScopePtr() );
        
    } else if (isLiteral(declBeginning + 3)) {
        
        node = new ASTLiteral(createLiteral(declBeginning + 3));
        
    }
    
    _ast.emplaceDeclaration(type, name, node);
    
}

void Parser::parseConstruct(unsigned long long constructBeginning, unsigned long long constructEnd) {
    
    std::string construct = _tokens[constructBeginning + 1].value;
    
    unsigned long long condEnd = constructBeginning + 2;
    
    
    ASTNode * condition;
    
    if (construct != "else") {
        
        if (_tokens[constructBeginning + 2].type == tokType::intLit or
            _tokens[constructBeginning + 2].type == tokType::numLit) {
            
            std::string type = _tokens[constructBeginning + 2].type == tokType::intLit ? "int" : "num";
            
            condition = new ASTLiteral(type, _tokens[constructBeginning + 2].value);
            
        }
        else if (_tokens[constructBeginning + 2].type == tokType::openingBra) {
            
            condition = new ASTMemberAccess(parseMemberAccess(constructBeginning + 2));
            condEnd = constructBeginning + 2;
            while (_tokens[condEnd].type != tokType::closingBra) {
                ++condEnd;
                if (condEnd == _tokens.size()) {
                    throw missing_token(']');
                }
            }
            
        }
        else if (_tokens[constructBeginning + 2].type == tokType::id){
            condition = new ASTVariable(_tokens[constructBeginning + 2].value, _ast.getCurrentScopePtr());
        }
        else {
            condEnd = findSexpEnd(constructBeginning + 2);
            condition = new ASTFunCall(parseFunCall(constructBeginning + 2, condEnd));
        }
        
    }
    else {
        condition = new ASTFunCall(_ast.getCurrentScopePtr(), "", std::vector<ASTNode *>());
        condEnd = constructBeginning + 1;
    }
    
    
    
    _ast.emplaceConstruct(construct, condition);
    
    /* Parse all sexps inside construct body */
    /* All sexps will be emplaced into construct body because emplaceConstruct changes current scope */
    parseSexps(condEnd + 1);
    
    /* Exit the scope of the construct */
    _ast.exitScope();
    
}

ASTFunCall Parser::parseFunCall(unsigned long long callBeginning, unsigned long long callEnd) {

    std::string name;
    std::vector<ASTNode *> params;
    
    name = _tokens[callBeginning + 1].value;
    
    for (unsigned long long iter = callBeginning + 2; iter < callEnd; ++iter) {
    
        if (_tokens[iter] == tokType::openingPar) {
            
            unsigned long long sexpEnd = findSexpEnd(iter);
            ASTFunCall * funcall = new ASTFunCall(parseFunCall(iter, sexpEnd));
            
            params.emplace_back(funcall);
            iter = sexpEnd;
            
        } else if (_tokens[iter] == tokType::id) {
            
            ASTVariable * var = new ASTVariable(_tokens[iter].value, _ast.getCurrentScopePtr());
            params.emplace_back(var);
            
        } else if (_tokens[iter] == tokType::openingBra) {
          
            ASTMemberAccess * attr = new ASTMemberAccess(parseMemberAccess(iter));
            params.emplace_back(attr);
            
        } else if (isLiteral(iter)) {
            
            ASTLiteral * lit = new ASTLiteral(createLiteral(iter));
            params.emplace_back(lit);
            
        }
        
    }
    
    return ASTFunCall(_ast.getCurrentScopePtr(), name, params);

}

ASTMemberAccess Parser::parseMemberAccess(unsigned long long exprBeginning) {
    
    std::vector<std::string> accessedMembers;
    
    unsigned long long iter = exprBeginning + 2;
    const size_t tokSize = _tokens.size();
    
    while (_tokens[++iter] != tokType::closingBra) {
        
        if (iter >= tokSize) {
            throw missing_token(']');
        }
        
        if (_tokens[iter] != tokType::id) {
            throw unexpected_token(_tokens[iter].value);
        }
        
        accessedMembers.emplace_back(_tokens[iter].value);
        
    }
    
    return ASTMemberAccess(accessedMembers, _ast.getCurrentScopePtr());
    
}

unsigned long long Parser::findSexpEnd(unsigned long long sexpBeginning) {
    
    unsigned long long sexpEnd = sexpBeginning;
    
    const unsigned long long tokensLen = _tokens.size();
    int parenCounter = 0;
    
    while (true) {
        
        /* Prevent the compiler from trying to access outside memory bounds */
        if (sexpEnd == tokensLen) {
            throw missing_token(')');
        }
        
        token & tok = _tokens[sexpEnd];
        
        if (tok == tokType::openingPar) {
            ++parenCounter;
        }
        else if (tok == tokType::closingPar) {
            --parenCounter;
        }
        
        if (not parenCounter) {
            return sexpEnd;
        }
        
        ++sexpEnd;
        
    }
    
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
        
        token & tok = _tokens[iter];
        
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

void Parser::globalVarDeclaration(unsigned long long declBeginning, unsigned long long declEnd) {
    
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
        else if (iter == tokensLen and parenCounter >= 0) {
            throw missing_token(')');
        }
        
        ++iter;
        
    } while (parenCounter >= 0);
    
    parameter param;
    std::vector<parameter> members;
    for (auto sexp : sexps) {
        
        /* If token isn't closing paren, it must be an opening parenthesis, which means s-exp is a method */
        if (_tokens[sexp + 3] != tokType::closingPar) {
            
            if (_tokens[sexp + 3] != tokType::openingPar) {
                throw unexpected_token(_tokens[sexp + 3].value);
            }
            
        }
        param = parseVariable(sexp);
        
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

void Parser::parseMethod(unsigned long long methodBeginning, const std::string & methodName) {
    
    const unsigned long long methodEnd = findSexpEnd(methodBeginning);
    
    
    
}

parameter Parser::parseVariable(unsigned long long sexpBeginning) {
    
    parameter param;
    
    param.type = _tokens[sexpBeginning + 1].value;
    param.name = _tokens[sexpBeginning + 2].value;
    
    if ( not _ast.isDataType(param.type) ) {
        throw invalid_declaration("Unknown data type " + param.type);
    }
    
    if (_tokens[sexpBeginning + 3] != tokType::closingPar) {
        
        throw invalid_declaration("Class attributes and global variables can't be initialized with default values. Initialize them using a function. ");
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
        
        if (_tokens[i].type != tokType::openingPar and not parens) {
            
            throw unexpected_token(_tokens[i].value);
            
        }
        
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
    
    
    if (_tokens[defBeginning + 1].value == "class") {
        
        classDefinition(defBeginning, defEnd);
        
    } else if ( _tokens[defBeginning + 3].type == tokType::openingPar ) {
        
        parseFun(defBeginning, defEnd);
    
    } else {
        
        /* Global variable declaration */
        globalVarDeclaration(defBeginning, defEnd);
        
    }
    
}

void Parser::parse() {
    
    parseDefinitions();
    
}
