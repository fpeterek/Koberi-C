//
//  parser.cpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#include "parser.hpp"


Parser::Parser(std::vector<token> & vectorRef, AbstractSyntaxTree & ast):
        _tokens(vectorRef),
        _ast(ast) /* Call reference constructors */ {
    
    _output.open("output.c");
    
    if (not _output) {
        
        throw file_not_opened("output.c");
        
    }
    
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

std::string Parser::getVarType(parameter & param) {
    
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

std::string Parser::getVarType(std::string & varName) {
    
    parameter param;
    param.value = varName;
    return getVarType(param);
    
}

void Parser::mangleName(std::string & name, std::vector<parameter> & params) {
    
    if (name == "main") { return; }
    
    name.insert(0, "_");
    
    for (auto & i : params) {
        
        name += "_" + i.type;
        
    }
    
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

parameter Parser::classAttributeAccess(unsigned long long sexpBeginning) {
    
    parameter attr;
    
    std::list<std::string> variables;
    
    std::string variableName;
    std::string className;
    
    unsigned long long iter = sexpBeginning + 1;
    
    while (_tokens[iter] != tokType::closingBra) {
        
        if (_tokens[iter] != tokType::id) {
            throw invalid_syntax("Error: Expected identifier in class attribute access operator[] ");
        }
        
        variables.push_back(_tokens[iter].value);
        
        ++iter;
        
    }
    
    if (variables.size() < 2) {
        throw invalid_syntax("Error: Expected identifier in class attribute access operator[] ");
    }
    
    attr.value = variables.front();
    className = getVarType(attr.value);
    
    variables.pop_front();
    
    while (variables.size()) {
        
        variableName = variables.front();
        
        variables.pop_front();
        
        try {
            
            /* Empty class object I can bind to the reference wrapper so I can avoid using pointers    */
            /* The reference is either overwritten or an exception is thrown so temp is never accessed */
            _class temp{};
            std::reference_wrapper<_class> c(temp);
            
            try {
                
                c = classes.at(className);
            
            } catch (const std::out_of_range & e) {
            
                throw not_a_class(className);
            
            }
            
            attr.type = c.get().getVarType(variableName);
            attr.value += "." + variableName;
            
            className = attr.type;
            
        } catch (const std::out_of_range & e) {
            
            throw no_such_member(variables.front(), className);
            
        }
        
    }
    
    return attr;
    
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
            
            params.emplace_back(classAttributeAccess(iter));
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
        _localVars.emplace(params[0].value, funName);
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
    
    for (unsigned long long i : sexps) {
        
        expr = fun(this, i);
        
        /*  Appending a semicolon at the end unless the expression is of type .cf (control flow)   */
        /*       In the end there might be some trailing semicolons, but that would just create    */
        /*       empty expressions which don't matter, unless they are between an if/else if/else  */
        /*  If you wanna help battle the semicolon inflation, don't comment your Kobeři-C code     */
        
        _output << expr.value << (expr.type != ".cf" ? ";" : "") << std::endl;
    
    }
    
}

void Parser::parseFun(unsigned long long funBeginning, unsigned long long funEnd) {
    
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
    
    std::function<parameter(Parser*, unsigned long long)> fun = &Parser::parseSexp;
    
    parseSexps(sexp + 1, fun);
    
    _output << "}\n\n";
    
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
    
    _output << ss.str() << std::endl;
    
    _globalVars.emplace(name, type);
    
}

void Parser::funDeclaration(unsigned long long declBeginning, unsigned long long declEnd) {
    
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
        /* Not doing this because it doesn't make sense */
        /* _output << "int main(int argc, const char * argv[]);" << std::endl; */
#ifdef OUTPUT_FUNCTION_DECLARATION
        print("int main(int argc, const char * argv[]);");
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

void Parser::classDeclaration(unsigned long long declBeginning, unsigned long long declEnd ) {
    
    std::string name = _tokens[declBeginning + 2].value;
    std::vector<parameter> params;
    
    /* Assume class doesn't inherit from anything so fourth token is paren */
    unsigned long long firstDeclaration = declBeginning + 5;
    
    /* 0th token is paren, first is class, second is name, third is opening paren, fourth is closing paren or superclass name */
    if (_tokens[declBeginning + 4] != tokType::closingPar) {
        
        /* Increment firstDeclaration by one to reflect the fact that this class inherits from a superclass */
        ++firstDeclaration;
        
        std::string superclass = _tokens[declBeginning + 4].value;
        
        try {
            
            params = classes.at(superclass).vars;
            
        } catch (const std::out_of_range & e) {
            
            throw undefined_class(superclass);
            
        }
        
    }
    
    if (_tokens[firstDeclaration - 1] != tokType::closingPar) {
        
        throw invalid_syntax("Classes can only inherit from 1 superclass. ");
        
    }
    
    _output << "\ntypedef struct " << name << " {\n";
    
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
        _output << "    " << expr::variableDeclaration(type, name).value << "\n";
    }
    
    _output << "} " << name << ";\n" << std::endl;
    
    _class c;
    c.vars = params;
    classes.emplace(name, c);
    dataTypes.emplace_back(name);
    
}

void Parser::parseDeclarations() {
    
    _output << "/* User defined function declarations and global variables. */\n\n";
    
    long long parens = 0;
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
    
    if (parens > 0) {
        throw missing_token(')');
    }
    if (parens < 0) {
        throw unexpected_token(')');
    }
    
    _output << "\n"; /* Output two newlines after function declarations, mostly for readability */
    
}

/* --- Definition example ---
 
 ;;; Function
 (int x ()
     (if (x)
          (print x " evaluates to true")))
 
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

void Parser::declaration(unsigned long long declBeginning, unsigned long long declEnd) {
    
    if (_tokens[declBeginning + 1].value == "class") {
        classDeclaration(declBeginning, declEnd);
        return;
    }
    
    /* Only allow literals */
    if (not (_tokens[declBeginning + 3].type == tokType::openingPar)) {
        varDeclaration(declBeginning, declEnd);
        return;
    }
    
    funDeclaration(declBeginning, declEnd);
    
}

void Parser::libs() {
    
    _output << "#include <stdio.h>\n"
            << "#include <stdlib.h>\n"
            << "#include <math.h>\n"
            << "#include <time.h>\n"
            << "#include <string.h>\n"
            << "\n"
            << std::endl;
    
}

void Parser::typedefs() {
    
    _output << "typedef double num;\n"
            << "typedef long long ll;\n"
            << std::endl;
    
}

/* These should be replaced with a standard library written in Kobeři-C        */
/* They probably aren't used anywhere anyway, so I'll comment them out for now */
/* I should remove them in the near future                                     */

/*
void Parser::functions() {
    
    _output << "// Kobeři-c standard library functions \n\n"
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
*/

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
 (int var (+ 10 10)) ; Allowed
 (int var2 10)       ; Allowed
 (int var3 ())       ; Not Allowed
 
 */

void Parser::parseDefinitions() {
    
    _output << "/* User defined function definitions */\n" << std::endl;
    
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
    
    libs();
    typedefs();
    // functions();
    
    parseDeclarations();
    parseDefinitions();
    
}