//
//  expressions.cpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#include "expressions.hpp"

std::string expr::variableDeclaration(std::string & type, std::string & name, std::string & value) {
    
    std::string val;
    val = type + " " + name + " = " + value + ";";
    return val;
    
}

std::string expr::setNumValue(std::string & var, std::string & value) {
    
    std::string val;
    val = var + " = " + value + ";";
    return val;
    
}

std::string expr::numericOperation(std::string & operation, std::vector<std::string> & nums) {
    
    if ( not nums.size() ) { return std::string(""); }
    if (nums.size() == 1 and operation == "-") { return "(-" + nums[0] + ")";  } /* (- x) gives -x */
    if (nums.size() == 1 ) { return nums[0]; }
    
    std::string val = "(" + nums[0];
    for ( int i = 1; i < nums.size() - 1; ++i ) {
        
        val += operation + nums[i];
        
    }
    val += operation + nums.back() + ")";
    
    return val;
    
}

std::string expr::numMod(std::vector<std::string> & nums) {
    
    if ( not nums.size() ) { return std::string(""); }
    if ( nums.size() == 1 ) { return nums[0]; }
    std::string val;
    
    
    val = "fmod(" + nums[0] + ", " + nums[1] + ")";
    
    auto iter = nums.begin();
    nums.erase(iter);
    nums.erase(iter);
    nums.insert(iter, val);
    
    return expr::numMod(nums);
    
}

std::string expr::print(std::vector<parameter> & params) {
    
    std::string string;
    
    for (auto & i : params) {
        
        if (i.type == "num") {
            string += "printf(\"%f\", " + i.value + ");\n";
        }
        else if (i.type == "int") {
            string += "printf(\"%d\", " + i.value + ");\n";
        }
        string += "fputs(" + i.value + ", stdout);\n";
        
    }
    
    return string;
    
}

std::string expr::conversionToNum(parameter & param) {
    
    if (param.type == "num") {
        return param.value;
    }
    else if (param.type == "int") {
        expr::intToNum(param);
    }
    else if (param.type == "str") {
        expr::strToNum(param);
    }
    
    return std::string();
    
}

std::string expr::intToNum(parameter & param) {
    return "(num)" + param.value;
}

std::string expr::strToNum(parameter & param) {
    return "atof(" + param.value + ")";
}

std::string expr::conversionToInt(parameter & param) {
    
    if (param.type == "num") {
        return expr::numToInt(param);
    }
    else if (param.type == "int") {
        return param.type;
    }
    else if (param.type == "str") {
        return expr::strToInt(param);
    }
    
    return std::string();
    
}

std::string expr::numToInt(parameter & param) {
    return "(ll)" + param.value;
}

std::string expr::strToInt(parameter & param) {
    return "atoll(" + param.value + ")";
}



