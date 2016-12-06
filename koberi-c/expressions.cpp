//
//  expressions.cpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#include "expressions.hpp"

parameter::parameter() {
    
    value = "";
    type = "";
    
}

parameter::parameter(const std::string & newValue) {
    
    value = newValue;
    
}

parameter::parameter(const std::string & newValue, const std::string & newType) {
    
    value = newValue;
    type = newType;
    
}

parameter expr::variableDeclaration(std::string & type, std::string & name, std::string & value) {
    
    return parameter(type + " " + name + " = " + value + ";", type);
    
}

parameter expr::variableDeclaration(std::string & type, std::string & name) {
    
    return parameter(type + " " + name + ";", type);
    
}

parameter expr::setNumValue(std::string & var, std::string & value) {
    
    parameter val( var + " = " + value + ";");
    return val;
    
}

parameter expr::binaryOperator(std::vector<parameter> & params, std::string & op) {
    
    if (op == "%" and params[0].type == "num") {
        return expr::numMod(params);
    }
    
    if (op == "set" and params.size() != 1) {
        std::string str = "(set";
        for (auto & i : params) {
            str += i.value;
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
        
    }
    val.value += " " + oper + " " + params.back().value + ")";
    
    return val;
    
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

parameter expr::print(std::vector<parameter> & params) {
    
    parameter string;
    string.type = "int";
    
    for (auto & i : params) {
        
        if (i.type == "num") {
            string.value += "\tprintf(\"%f\", " + i.value + ");\n";
        }
        else if (i.type == "int") {
            string.value += "\tprintf(\"%lld\", " + i.value + ");\n";
        } else {
            string.value += "\tfputs(" + i.value + ", stdout);\n";
        }
        
    }
    
    return string;
    
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

