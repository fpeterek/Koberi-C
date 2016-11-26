//
//  contains.hpp
//  koberi-c
//
//  Created by Filip Peterek on 26/11/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#ifndef contains_hpp
#define contains_hpp

#include <stdio.h>
#include <vector>
#include <array>
#include <iostream>


/* Returns true if item can be found in a vector, otherwise returns false */

/* std::vector template accepts type and allocator, allocator will be inferred automatically, but the template still requires it */

template<typename T, typename A, template<typename, typename> class C>
bool operator >> (const C<T, A> & container, const T & item) {
    
    for (auto & iter : container) {
        
        if (iter == item) { return true; }
        
    }
    
    return false;
    
}

/* Damn C strings */

template <typename A, template<typename, typename> class C>
bool operator >> (const C<std::string, A> & container, const char * str) {
    
    for (auto & i : container) {
        
        if (i == str) { return true; }
        
    }
    
    return false;
    
}

#endif /* contains_hpp */
