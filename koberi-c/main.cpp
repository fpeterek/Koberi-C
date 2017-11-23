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
    
#ifndef DEBUG_COMPILE_TEST
    if (argc < 2) {
        std::cout << "Error: No file provided. Compilation terminated. " << std::endl;
        return -1;
    }
    
    std::vector<std::string> args;
    
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }
    
    std::string & file = args[0];
    
    for (auto & a : args) {
        if (a == "-v") {
            expr::setVerbose(true);
        }
        else {
            std::cout << "Unknown parameter: " << a << std::endl;
            return -1;
        }
    }
    
#endif
    
#ifdef SET_VERBOSE
    expr::setVerbose(true);
#endif
    
    KoberiC ks;
    
    try {
#ifdef DEBUG_COMPILE_TEST
        ks.compile(TEST);
#else
        ks.compile(file);
#endif
    } catch (std::exception & e) {
        std::cout << e.what() << "\n" << std::endl;
        std::cout << "Compilation terminated. " << std::endl;
    }
    
#ifdef PRINT_TOKENS
    ks.test();
#endif
    
    return 0;
    
}

