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

AASTOperator * expr::mod(const std::vector<AASTNode *> & params) {
    
    std::string op = "%";
    
    for (auto & param : params) {
        
        /* If at least one of the parameters is a floating point number, call fmod() */
        if (not expr::isNumericalType(param->type())) {
            throw invalid_call("mod", "Error: can't call mod on non-numerical types. ");
        }
        
        if (param->type() == "num") {
            op = "fmod";
        }
        
    }
    
    return new AASTOperator(op, "num", params);
    
}

AASTOperator * expr::comparison(const std::vector<AASTNode *> & params, const std::string & op) {
    
    /* Check if parameters are of numerical or pointer type, therefore comparable */
    for (auto & param : params) {
        
        if (not ( expr::isNumericalType(param->type()) or syntax::isPointerType(param->type()) ) ) {
            throw invalid_call(op, "Error: Only numerical or pointer types are comparable. ");
        }
        
    }
    
    return new AASTOperator(op, "int", params);
    
}

AASTOperator * expr::binaryOperator(const std::vector<AASTNode *> & params, const std::string & op) {
    
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
    
    return new AASTOperator(binary_operators_map.at(op), params[0]->type(), params);
    
}

AASTOperator * expr::set(const AASTNode * lvalue, const AASTNode * rvalue) {
    
    /* If one value is a pointer and the other isn't dereference the pointer and check types */
    
    /* If value on left is a pointer */
    if (syntax::isPointerType(lvalue->type()) and not syntax::isPointerType(rvalue->type())) {
        
        /* Get type of dereferenced left value */
        std::string lvalueType = lvalue->type();
        lvalueType.pop_back();
        
        /* Derefence value on the left side */
        AASTOperator * derefLvalue = new AASTOperator("&", lvalueType,
                                                 std::vector<AASTNode *>( { const_cast<AASTNode *>(lvalue) } ));
        
        /* Throw if types aren't equal */
        if (not (isNumericalType(lvalueType) and isNumericalType(rvalue->type()))
            and lvalueType != rvalue->type()) {
            throw type_mismatch("Can't assign value of type " + rvalue->type() + " to variable of type " + lvalueType);
        }
        
        return new AASTOperator("=",
                            lvalueType,
                            std::vector<AASTNode *>( { derefLvalue,
                                                       const_cast<AASTNode *>(rvalue)
                                                     }));
        
    }
    /* If value on right is a pointer */
    if (not syntax::isPointerType(lvalue->type()) and syntax::isPointerType(rvalue->type())) {
        
        /* Get type of dereferenced right value */
        std::string rvalueType = rvalue->type();
        rvalueType.pop_back();
        
        /* Derefence value on the right side */
        AASTOperator * derefRvalue = new AASTOperator("&", rvalueType,
                                                      std::vector<AASTNode *>( { const_cast<AASTNode *>(rvalue) } ));
        
        /* Throw if types aren't equal */
        if (not (isNumericalType(rvalueType) and isNumericalType(lvalue->type()))
            and rvalueType != lvalue->type()) {
            throw type_mismatch("Can't assign value of type " + lvalue->type() + " to variable of type " + rvalueType);
        }
        
        return new AASTOperator("=",
                            lvalue->type(),
                            std::vector<AASTNode *>( { const_cast<AASTNode *>(lvalue),
                                                       derefRvalue,
                                                     }));
        
    }
    
    /* Check if types are equal */
    if (not (isNumericalType(lvalue->type()) and isNumericalType(rvalue->type()))
        and lvalue->type() != rvalue->type()) {
        throw type_mismatch("Can't assign value of type " + lvalue->type() + " to variable of type " + rvalue->type());
    }
    
    return new AASTOperator("=",
                        lvalue->type(),
                        std::vector<AASTNode *>( { const_cast<AASTNode *>(lvalue),
                                                   const_cast<AASTNode *>(rvalue)
                                                 } ));
    
}


AASTOperator * expr::unaryOperator(const AASTNode * param, const std::string & op) {
    
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
        return dereference(param);
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
    
    return new AASTOperator(unary_operators_map.at(op), type, params);
    
}

AASTOperator * expr::parameterless_operator(const std::string & op) {
    return new AASTOperator(op, "void", std::vector<AASTNode*>());
}

AASTOperator * expr::dereference(const AASTNode * param) {
    
    std::string type = param->type();
    
    std::vector<AASTNode *> params;
    AASTNode * p = const_cast<AASTNode*>(param);
    params.emplace_back(p);
    
    if (syntax::isPointerType(type)) {
        return new AASTOperator("&", type, params);
    }
    
    return new AASTOperator("&", syntax::pointerForType(type), params);
    
}
