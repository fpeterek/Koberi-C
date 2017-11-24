//
//  debug_macro_definitions.hpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#ifndef debug_macro_definitions_hpp
#define debug_macro_definitions_hpp


/* This file includes macro definitions used for debugging */

#define DEBUG_COMPILE_TEST // Compiles file testing_script.koberice instead of compiling file provided via command line args
#define SET_VERBOSE // Automatically turns on verbose mode
// #define PRINT_TOKENS // Print tokens to stdout after input is tokenized
// #define PRINT_LINE  // Print lines to stdout after reading them from file
// #define PRINT_FUNCTION_TOKENS // Print tokens of a function
// #define PRINT_FUNCTION // Print function to stdout
#define OUTPUT_FUNCTION_DECLARATION // Print function declaration to stdout
// #define OUTPUT_GLOBAL_VARIABLE_DECLARATION // Print global variable declaration to stdout
#define OUTPUT_GLOBAL_SCOPE_NODES // Print information about nodes from the global scopes to stdout

#endif /* debug_macro_definitions_hpp */
