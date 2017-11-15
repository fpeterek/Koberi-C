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
#include "analyzed_abstract_syntax_tree.hpp"
#include "name_mangler.hpp"
#include "expressions.hpp"
#include "syntax.hpp"

#include "debug_macro_definitions.hpp"

#define INDENT "    " /* Uses four spaces to indent */

class Translator {
    
    TraversableAbstractSyntaxTree & _ast;
    AnalyzedAbstractSyntaxTree & _aast;
    
    /* Used to store name of current function to make error messages slightly more useful */
    std::string _functionName;

    
    void main();
    
    std::ofstream _output;
    
    void libraries();
    void typedefs();
    void translateClasses();
    void translateFunctionDeclarations();
    void translateGlobals();
    void translateFunctions();
    
    
public:
    
    Translator(TraversableAbstractSyntaxTree & ast, AnalyzedAbstractSyntaxTree & aast);
    
    void setOutputFile(const std::string & filename);
    
    void translate();
    
};

#endif /* translator_hpp */
