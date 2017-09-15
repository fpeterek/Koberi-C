//
//  koberi-c.hpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#ifndef koberi_c_hpp
#define koberi_c_hpp

/* Most of these are most likely included already, but whatever */
/*                                                              */
/*                    ( [compiler do-magic] )                   */
/*                                                              */
/****************************************************************/

#include <stdio.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <cctype>
#include <array>

#include "tokenizer.hpp"
#include "expressions.hpp"
#include "print.hpp"
#include "exceptions.hpp"
#include "token.hpp"
#include "syntax.hpp"
#include "parser.hpp"
#include "traversable_ast.hpp"
#include "translator.hpp"
#include "import_system.hpp"

#include "debug_macro_definitions.hpp"


class KoberiC {
    
    std::vector<token> _tokens;
    
    ImportSystem _importSystem;
    Tokenizer _tokenizer;
    Parser _parser;
    TraversableAbstractSyntaxTree _ast;
    Translator _translator;
    
    
    std::string _filename;
    
    std::vector<std::string> files;
    
    void parseInputFileName(const std::string & filename);
    
    void handleImports(const std::string & file);
    void tokenize(const std::string & filename);
    void parse();
    void translate();
    
public:
    
    KoberiC();
    
    void compile(const std::string & filename);
    
    void test();
    
};

#endif /* koberi_c_hpp */
