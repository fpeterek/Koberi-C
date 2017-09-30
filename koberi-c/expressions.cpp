//
//  expressions.cpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#include "expressions.hpp"

bool expr::isConstruct(const std::string & construct) {
    
    return contains(constructs, construct);
    
}

bool expr::isOperator(const std::string & op) {
    
    return contains(operators, op);
    
}

bool expr::isParameterlessOperator(const std::string & op) {
    
    return contains(parameterless_operators, op);

}

bool expr::isUnaryOperator(const std::string & op) {
    
    return contains(unary_operators, op);

}

bool expr::isBinaryOperator(const std::string & op) {
    
    return contains(binary_operators, op);

}

parameter expr::variableDeclaration(std::string & type, std::string & name, std::string & value) {
    
    return parameter((type == "int" ? "ll" : type) + " " + name + " = " + value + ";", type);
    
}

parameter expr::variableDeclaration(std::string & type, std::string & name) {
    
    return parameter((type == "int" ? "ll" : type) + " " + name + ";", type);
    
}

parameter expr::setNumValue(std::string & var, std::string & value) {
    
    parameter val( var + " = " + value + ";");
    return val;
    
}

parameter expr::binaryOperator(std::vector<parameter> & params, std::string & op) {
    
    if (op == "mod") {
        
        for (auto & param : params) {
            
            /* If at least one of the parameters is a floating point number, call fmod() */
            if (param.type == "num") {
                return expr::numMod(params);
            }
            
        }
        
    }
    
    if (op == ">" or op == "<" or op == "equals" or op == "not_eq") {
        return comparison(op, params);
    }
    
    if (op == "set" and params.size() != 2) {
        std::string str = "(set";
        for (auto & i : params) {
            str += " " + i.value;
        }
        str += ")";
        throw invalid_operator(str);
    }
    
    parameter val;
    val.type = "void";
    
    const std::string oper = binary_operators_map.at(op);
    
    if ( not params.size() ) { return val; }
    
    if (params.size() == 1 and op == "-") {
        val.value = "(-" + params[0].value + ")";
        val.type = params[0].type;
        return val;
    } /* (- x) gives -x */
    
    if (params.size() == 1 ) { val = params[0]; return val; }
    
    val.value = "(" + params[0].value;
    val.type = params[0].type;
    for ( int i = 1; i < params.size() - 1; ++i ) {
        
        val.value += " " + oper + " " + params[i].value;
        
        if (params[i].value == "num") {
            val.value = "num";
        }
        
    }
    val.value += " " + oper + " " + params.back().value + ")";
    
    return val;
    
}

/* Can't use a lambda expressions because different lambdas have different types, so I'm creating two helper */
/* Functions which are only called from one function, don't belong to the expr namespace, aren't declared    */
/* In expressions.hpp and should never be called                                                             */

/* ----------------------------------------------------- Don't use ----------------------------------------------------- */
/* - */                                                                                                             /* - */
/* - */ void equals(parameter & expr, const std::string & op, std::vector<parameter> & params, const size_t iter) { /* - */
/* - */     expr.value += " " + params[0].value + " " + op + " " + params[iter].value + " &&";                      /* - */
/* - */ }                                                                                                           /* - */
/* - */                                                                                                             /* - */
/* - */ void ltOrGt(parameter & expr, const std::string & op, std::vector<parameter> & params, const size_t iter) { /* - */
/* - */     expr.value += " " + params[iter - 1].value + " " + op + " " + params[iter].value + " &&";               /* - */
/* - */ }                                                                                                           /* - */
/* - */                                                                                                             /* - */
/* ----------------------------------------------------- Don't use ----------------------------------------------------- */

parameter expr::comparison(std::string & op, std::vector<parameter> & params) {
    
    const size_t paramsSize = params.size();
    
    if (paramsSize < 2) {
        
        std::string expr = "(" + op;
        for (auto & i : params) {
            expr += i.value;
        }
        expr += ")";
        throw invalid_operator(expr);
        
    }
    
    parameter expr;
    expr.type = "int";
    
    expr.value = "(";
    
    /* Can't use a lambda expressions because different lambdas have different types, so I'm using an std::function */
    auto fun = (op == "equals" or op == "not_eq") ? equals : ltOrGt;
    
    for (size_t i = 1; i < paramsSize; ++i) {
        
        fun(expr, binary_operators_map.at(op), params, i);
        
    }
    
    /* Also add 1 at the end of the expression, because the main loop in this function adds the and operator */
    /* So there's a trailing and at the end of the expression, adding 1 fixes this in a simple way without   */
    /* Making me change the loop to something more complicated                                               */
    /* The compiler will optimize this anyway                                                                */
    expr.value += " 1 )";
    
    return expr;
    
}

parameter expr::numMod(std::vector<parameter> & nums) {
    
    parameter val;
    val.type = "num";
    
    if ( not nums.size() ) { return val; }
    if ( nums.size() == 1 ) { val.value = nums[0].value; return val; }
    
    val.value = "fmod(" + nums[0].value + ", " + nums[1].value + ")";
    
    auto iter = nums.begin();
    nums.erase(iter);
    nums.erase(iter);
    nums.insert(iter, val.value);
    
    return numMod(nums);
    
}

std::vector<std::string> expr::print(std::vector<parameter> & params) {
    
    std::vector<std::string> statements;
    
    for (auto & i : params) {
        
        if (i.type == "num") {
            statements.emplace_back("printf(\"%f\", " + i.value + ")");
        }
        else if (i.type == "int") {
            statements.emplace_back("printf(\"%lld\", " + i.value + ")");
        }
        else if (i.type == "str") {
            statements.emplace_back("fputs(\"" + i.value + "\", stdout)");
        }
        else {
            throw invalid_syntax("(print) can only print primitive types to stdout. ");
        }
        
    }
    
    return statements;
    
}

/* Replaces escaped quotation marks in strings so you can use string literals in inline c */
void replaceEscapeSequence(std::string & str) {
    
    size_t position;
    
    while ( (position = str.find("\\\"")) != std::string::npos ) {
        str.replace(position, 2, "\"");
    }
    
}

std::vector<std::string> expr::inlineC(std::vector<parameter> &params) {
    
    std::vector<std::string> c;
    
    std::string temp;
    for (auto & i : params) {
        
        if (i.type != "str") {
            throw bad_type("Inline C requires a string literal. ");
        }
        
        temp = i.value;
        replaceEscapeSequence(temp);
        c.emplace_back(temp);
        
    }
    
    return c;
    
}

parameter expr::conversionToNum(parameter & param) {
    
    if (param.type == "num") {
        return param;
    }
    else if (param.type == "int") {
        return intToNum(param);
    }
    else if (param.type == "str") {
        return strToNum(param);
    }
    
    return parameter();
    
}

parameter expr::intToNum(parameter & param) {
    parameter val("(num)" + param.value, "num");
    return val;
}

parameter expr::strToNum(parameter & param) {
    parameter val("atof(" + param.value + ")", "num");
    return val;
}

parameter expr::voidToNum(parameter & param) {
    
    throw bad_type("Invalid conversion of type void to type num. ");
    
}

parameter expr::conversionToInt(parameter & param) {
    
    if (param.type == "num") {
        return numToInt(param);
    }
    else if (param.type == "int") {
        return param;
    }
    else if (param.type == "str") {
        return strToInt(param);
    }
    
    return parameter();
    
}

parameter expr::numToInt(parameter & param) {
    parameter val("(ll)" + param.value, "int");
    return val;
}

parameter expr::strToInt(parameter & param) {
    parameter val("atoll(" + param.value + ")", "int");
    return val;
}

parameter expr::voidToInt(parameter & param) {
    throw bad_type("Invalid conversion of type void to type int. ");
}

parameter expr::unaryOperator(parameter & param, std::string & op) {
    
    /* Type stays the same, value changes */
    parameter val = param;
    std::string oper = unary_operators_map.at(op);
    val.value = oper + " ( " + param.value + " )";
    return val;
    
}

parameter expr::parameterless_operator(std::string & op) {
    parameter param(op, "void");
    return param;
}
