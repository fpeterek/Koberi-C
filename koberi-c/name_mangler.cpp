//
//  name_mangler.cpp
//  koberi-c
//
//  Created by Filip Peterek on 23/07/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#include "name_mangler.hpp"

const std::string NameMangler::prefix = "_koberic_";

std::string NameMangler::mangleName(const std::string & name, const std::vector<parameter> & params) {
    
    std::string mangledName = prefix + name;
    
    for (const auto & param : params) {
        
        mangledName.append("_" + param.type);
        
        /* Function names can't contain asterisks, so a different way of indicating */
        /* that the parameter is a pointer is needed                                */
        /* Append __ptr if parameter is a pointer                                   */
        if (mangledName.back() == '*') {
            mangledName.pop_back();
            mangledName.append("__ptr");
        }
        
    }
    
    return mangledName;
    
    
}

std::string NameMangler::mangleName(const std::string & name, const std::vector<std::string> & paramTypes) {
    
    std::string mangledName = prefix + name;
    
    for (const auto & param : paramTypes) {
        
        mangledName.append("_" + param);
        
        /* Function names can't contain asterisks, so a different way of indicating */
        /* that the parameter is a pointer is needed                                */
        /* Append __ptr if parameter is a pointer                                   */
        if (mangledName.back() == '*') {
            mangledName.pop_back();
            mangledName.append("__ptr");
        }
        
    }
    
    return mangledName;
    
}

std::string NameMangler::premangleMethodName(const std::string & name, const std::string & className) {
    
    std::string premangledName = className + "_m_" + name;
    return premangledName;
    
}
