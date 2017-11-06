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

bool expr::isNumericalType(const std::string & type) {
    
    return contains(numerical_types, type);
    
}

parameter expr::variableDeclaration(std::string & type, std::string & name, std::string & value) {
    
    return parameter((type == "int" ? syntax::intType : type) + " " + name + " = " + value + ";", type);
    
}

parameter expr::variableDeclaration(std::string & type, std::string & name) {
    
    return parameter((type == "int" ? syntax::intType : type) + " " + name + ";", type);
    
}

parameter expr::setNumValue(std::string & var, std::string & value) {
    
    parameter val( var + " = " + value + ";");
    return val;
    
}

AASTOperator expr::mod(const std::vector<AASTNode *> & params) {
    
    for (auto & param : params) {
        
        /* If at least one of the parameters is a floating point number, call fmod() */
        if (not expr::isNumericalType(param->type())) {
            throw invalid_call("mod", "Error: can't call mod on non-numerical types. ");
        }
        
        if (param->type() == "num") {
            return AASTOperator("fmod", "num", params);
        }
        
    }
    
    return AASTOperator("%", "num", params);
    
}

AASTOperator expr::comparison(const std::vector<AASTNode *> & params, const std::string & op) {
    
    /* Check if parameters are of numerical or pointer type, therefore comparable */
    for (auto & param : params) {
        
        if (not (expr::isNumericalType(param->type()) or syntax::isPointerType(param->type()))) {
            throw invalid_call("mod", "Error: can't call mod on non-numerical types. ");
        }
        
    }
    
    return AASTOperator("fmod", "num", params);
    
}

AASTOperator expr::binaryOperator(const std::vector<AASTNode *> & params, const std::string & op) {
    
    if (op == "mod") {
        return mod(params);
    }
    
    if (op == ">" or op == ">=" or op == "<" or op == "<=" or op == "equals" or op == "not_eq") {
        return comparison(params, binary_operators_map.at(op));
    }
    
    if (op == "set" and params.size() != 2) {
        std::string str = "(set";
        for (auto & i : params) {
            str += " " + i->value();
        }
        str += ")";
        throw invalid_operator(str);
    }
    
    if (op == "set") {
        
        const AASTNode * lvalue = params[0];
        const AASTNode * rvalue = params[1];
        return set(lvalue, rvalue);
        
    }
    
    const std::string oper = binary_operators_map.at(op);
    
    return AASTOperator(binary_operators_map.at(op), params[0]->type(), params);
    
}

AASTOperator expr::set(const AASTNode * lvalue, const AASTNode * rvalue) {
    
    /* If one value is a pointer and the other isn't dereference the pointer and check types */
    
    /* If value on left is a pointer */
    if (lvalue->type().back() == syntax::pointerChar and
        lvalue->type().back() != syntax::pointerChar) {
        
        /* Get type of dereferenced left value */
        const std::string lvalueType = lvalue->type().substr(0, lvalue->type().size() - 1);
        
        /* Derefence value on the left side */
        AASTOperator * derefLvalue = new AASTOperator("&", lvalueType,
                                                 std::vector<AASTNode *>( { const_cast<AASTNode *>(lvalue) } ));
        
        /* Throw if types aren't equal */
        if (not (isNumericalType(lvalueType) and isNumericalType(rvalue->type()))
            or lvalueType != rvalue->type()) {
            throw type_mismatch("Can't assign value of type " + rvalue->type() + " to variable of type " + lvalueType);
        }
        
        return AASTOperator("=",
                            lvalueType,
                            std::vector<AASTNode *>( { derefLvalue,
                                                       const_cast<AASTNode *>(rvalue)
                                                     }));
        
    }
    /* If value on right is a pointer */
    else if (lvalue->type().back() != syntax::pointerChar and
               rvalue->type().back() == syntax::pointerChar) {
        
        /* Get type of dereferenced right value */
        const std::string rvalueType = rvalue->type().substr(0, rvalue->type().size() - 1);
        
        /* Derefence value on the right side */
        AASTOperator * derefRvalue = new AASTOperator("&", rvalueType,
                                                      std::vector<AASTNode *>( { const_cast<AASTNode *>(rvalue) } ));
        
        /* Throw if types aren't equal */
        if (not (isNumericalType(rvalueType) and isNumericalType(lvalue->type()))
            or rvalueType != lvalue->type()) {
            throw type_mismatch("Can't assign value of type " + lvalue->type() + " to variable of type " + rvalueType);
        }
        
        return AASTOperator("=",
                            lvalue->type(),
                            std::vector<AASTNode *>( { const_cast<AASTNode *>(lvalue),
                                                       derefRvalue,
                                                     }));
        
    }
    
    /* Check if types are equal */
    if (not (isNumericalType(lvalue->type()) and isNumericalType(rvalue->type()))
        or lvalue->type() != rvalue->type()) {
        throw type_mismatch("Can't assign value of type " + lvalue->type() + " to variable of type " + rvalue->type());
    }
    
    return AASTOperator("=",
                        lvalue->type(),
                        std::vector<AASTNode *>( { const_cast<AASTNode *>(lvalue),
                                                   const_cast<AASTNode *>(rvalue)
                                                 } ));
    
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
        else if (i.type == "uint") {
            statements.emplace_back("printf(\"%llu\", " + i.value + ")");
        }
        else if (i.type == syntax::pointerForType("char")) {
            statements.emplace_back("fputs(" + i.value + ", stdout)");
        }
        else if (i.type == "char" or i .type == "uchar") {
            statements.emplace_back("putchar(" + i.value + ")");
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
        
        if (i.type != syntax::pointerForType("char")) {
            throw bad_type("Inline C requires a string literal. ");
        }
        
        /* Char* literals are surrounded with quotes. Remove the quotes. */
        temp = i.value.substr(1);
        temp.pop_back();
        replaceEscapeSequence(temp);
        c.emplace_back(temp);
        
    }
    
    return c;
    
}

AASTOperator expr::unaryOperator(const AASTNode * param, const std::string & op) {
    
    std::string type;
    
    if (op == "compl" and param->type() == "num") {
        invalid_parameter("Invalid parameter in call (compl " + param->value() +
                          "): Operator compl doesn't accept parameters of type num");
    }
    
    /* Return can't be used inside an expression to return a value because it jumps out of a function       */
    /* Arithmetical negation, binary complement incrementing and decrementing return value of the same type */
    /* Logical negation and size_of return an integer                                                       */
    if (op == "return") {
        type = "void";
    } else if (op == "-" or op == "inc" or op == "dec" or op == "compl") {
        type = param->type();
    } else if (op == "&") {
        type = param->type() + syntax::pointerChar;
    } else {
        type = syntax::intType;
    }
    
    if (op == "-") {
        if (not isNumericalType(op)) {
            invalid_call("(" + op + " " + param->value() + ")", "Unary operator - must receive a parameter of numerical type");
        }
    }
    std::vector<AASTNode *> params;
    AASTNode * p = const_cast<AASTNode*>(param);
    params.emplace_back(p);
    
    return AASTOperator(unary_operators_map.at(op), type, params);
    
}

AASTOperator expr::parameterless_operator(const std::string & op) {
    return AASTOperator(op, "void", std::vector<AASTNode*>());
}
