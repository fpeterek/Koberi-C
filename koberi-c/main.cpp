//
//  main.cpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#include "koberi-c.hpp"

static const std::string TEST = "testing_script.koberice";

int main(int argc, const char * argv[]) {
    
    /*if (argc < 2) {
        std::cout << "Error: No file provided. Compilation terminated. " << std::endl;
        return -1;
    }
    
    std::string file = argv[1]; */
    
    KoberiC ks;
    
    try {
        ks.compile(TEST);
        // ks.compile(file);
    } catch (std::exception & e) {
        print(e.what());
    }
    
#ifdef PRINT_TOKENS
    ks.test();
#endif
    
     // */
     
    return 0;
    
}
