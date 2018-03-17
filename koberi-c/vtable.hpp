//
//  vtable.hpp
//  koberi-c
//
//  Created by Filip Peterek on 17/03/2018.
//  Copyright © 2018 Filip Peterek. All rights reserved.
//

#ifndef vtable_hpp
#define vtable_hpp

#include <string>
#include <unordered_map>

struct _method {
    
    /* Vtable index */
    size_t pointerIndex;
    
    /* Actual type of function pointer, eg. void (*)(class*) */
    std::string pointerType;
    
    /* Holds info about method owner */
    std::string className;
    
};

typedef std::unordered_map<std::string /* mangledName */, _method> VTable;

#endif /* vtable_hpp */
