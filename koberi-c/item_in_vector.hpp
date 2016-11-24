//
//  item_in_vector.hpp
//  koberi-c
//
//  Created by Filip Peterek on 24/11/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#ifndef item_in_vector_hpp
#define item_in_vector_hpp

#include <vector>

/* Returns true if item can be found in a vector, otherwise returns false */

template <class T>
bool operator>>(std::vector<T> & vector, T & item) {
    
    for (T & iter : vector) {
        
        if (iter == item) { return true; }
        
    }
    
    return false;
    
}

#endif /* item_in_vector_hpp */
