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

std::string expr::numOperation(std::string & operation, std::vector<std::string> & nums) {
    
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

std::string expr::mod(std::vector<std::string> & nums) {
    
    if ( not nums.size() ) { return std::string(""); }
    if ( nums.size() == 1 ) { return nums[0]; }
    std::string val;
    
    
    val = "fmod(" + nums[0] + ", " + nums[1] + ")";
    
    auto iter = nums.begin();
    nums.erase(iter);
    nums.erase(iter);
    nums.insert(iter, val);

    
    return expr::mod(nums);
    
}

std::string expr::print(std::vector<std::string> & str) {
    
    std::string string;
    
    for (unsigned int i = 0; i < str.size(); ++i) {
        
        string += "fputs(" + str[i] + ", stdout);\n";
        
    }
    
    return string;
    
}








