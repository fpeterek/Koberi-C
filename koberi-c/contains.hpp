//
//  contains.hpp
//  koberi-c
//
//  Created by Filip Peterek on 26/11/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#ifndef contains_hpp
#define contains_hpp

/* Returns true if an item can be found inside a collecion, otherwise returns false          */
/* Works on any collection which implements iterators and holds                              */
/* types which implement the equality operator                                               */
/* Shouldn't be used on trees (eg. std::map) as trees don't require iteration over all items */

template <typename Collection, typename Type>
bool contains(const Collection & collection, const Type & item) {
    
    for (auto & i : collection) {
        
        if (item == i) { return true; }
        
    }
    
    return false;
    
}

#endif /* contains_hpp */
