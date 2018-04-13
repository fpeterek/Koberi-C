//
//  class.hpp
//  koberi-c
//
//  Created by Filip Peterek on 02/04/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#ifndef class_hpp
#define class_hpp

#include <string>
#include <vector>
#include <unordered_map>

#include "parameter.hpp"
#include "exceptions.hpp"
#include "vtable.hpp"

/* Class struct                                                                           */
/* Abstraction for Kobeři-C classes, all classes are stored as an instance of this struct */
/* Allows for easy look-up of methods/attributes and superclasses                         */

#define VTABLE_INIT "_vtable_init"

struct _class {
    
    std::string className;
    std::string superClass; 
    
    /* Implementing this as a vector because I want the attributes sorted */
    std::vector<parameter> attributes;
    
    /* Only holds methods of a certain class, but not methods of it's superclasses */
    std::unordered_map<std::string /* mangledName */, std::string> methods;
    
    /* Holds vtable with all methods, including inherited methods */
    VTable vtable;
    
    std::string getVarType(const std::string & name) const;
    bool hasVar(const std::string & name) const;
    
};

#endif /* class_hpp */
