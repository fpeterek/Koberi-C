//
//  translator.hpp
//  koberi-c
//
//  Created by Filip Peterek on 22/07/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#ifndef translator_hpp
#define translator_hpp

#include <fstream>
#include <iostream>
#include <vector>

#include "traversable_ast.hpp"
#include "name_mangler.hpp"

#include "debug_macro_definitions.hpp"

class Translator {
    
    TraversableAbstractSyntaxTree & _ast;
    
    std::ofstream _output;
    
    void typedefs();
    void translateClasses();
    void translateGlobalVars();
    void forwardFunctionDeclarations();
    /* Iterates over nodes and calls translateFunction() on each function */
    void translateFunctions();
    
    void main();
    
    /* Checks whether main function exists and follows Kobeři-C rules for (int main ()) */
    void kobericMainCheck();
    
    /* Translates a Kobeři-C function into a C function. */
    void translateFunction(ASTFunction & function);
    
public:
    
    Translator(TraversableAbstractSyntaxTree & ast);
    
    void setOutputFile(const std::string & filename);
    
    void translate();
    
    void test();
    
};

#endif /* translator_hpp */
