//
//  main.cpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#include "koberi-c.hpp"

#ifdef DEBUG_COMPILE_TEST
// static const std::string TEST = "testing_script.koberice";
static const std::string TEST = "power.koberice";
#endif

int main(int argc, const char * argv[]) {
    
#ifndef DEBUG_COMPILE_TEST
    
    /* Parse command line arguments */
    
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
    /* Verbose mode is automatically on when this macro is defined. Used for debugging purposes. */
    expr::setVerbose(true);
#endif
    
    KoberiC ks;
    
    try {
        /* If this macro is defined, compile a testing file, don't parse cl args, used for debugging */
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

