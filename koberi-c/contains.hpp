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

template <typename Container, typename Type>
bool operator >> (const Container & container, const Type & item) {
    
    for (auto & i : container) {
        
        if (item == i) { return true; }
        
    }
    
    return false;
    
}

template <typename Container, typename Type>
bool contains(const Container & container, const Type & item) {
    
    for (auto & i : container) {
        
        if (item == i) { return true; }
        
    }
    
    return false;
    
}

#endif /* contains_hpp */
