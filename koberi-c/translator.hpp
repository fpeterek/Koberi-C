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
#include <vector>

#include "traversable_ast.hpp"
#include "analyzed_abstract_syntax_tree.hpp"
#include "name_mangler.hpp"
#include "expressions.hpp"
#include "syntax.hpp"

#include "debug_macro_definitions.hpp"

#define INDENT "    " /* Uses four spaces to indent */

/* AnalyzedAST nodes are self translating */
/* Translator handles #include directives, makes sure all nodes are translated          */
/* in correct order, all classes are defined, all functions are forward declared etc... */

class Translator {
    
    TraversableAbstractSyntaxTree & _ast;
    AnalyzedAbstractSyntaxTree & _aast;
    
    /* Used to store name of current function to make error messages slightly more useful */
    std::string _functionName;

    /* Creates C int main(int, char**) function, which calls Kobeři-C main function (int main ()) */
    void main();
    
    /* Creates memory allocator function, which allocates memory and initializes v-tables */
    /* V-table initializer function is passed via a pointer                               */
    void memoryAllocator();
    
    /* Creates function which copies objects while preserving vtable pointers */
    void objectCopyFun();
    
    /* Output file stream */
    std::ofstream _output;
    
    /* Outputs library imports */
    void libraries();
    /* Outputs typedefs */
    void typedefs();
    /* Translates classes */
    void translateClasses();
    /* Translates forward function declarations */
    void translateFunctionDeclarations();
    /* Creates vtables for classes */
    void translateVtables();
    /* Translates global variable declarations */
    void translateGlobals();
    /* Translates functions and member functions */
    void translateFunctions();
    
    
public:
    
    Translator(TraversableAbstractSyntaxTree & ast, AnalyzedAbstractSyntaxTree & aast);
    
    /* Sets output file and opens file stream */
    void setOutputFile(const std::string & filename);
    
    void translate();
    
};

#endif /* translator_hpp */
