//
//  koberi-c.hpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#ifndef koberi_c_hpp
#define koberi_c_hpp

#include <iostream>
#include <vector>

#include "tokenizer.hpp"
#include "expressions.hpp"
#include "exceptions.hpp"
#include "token.hpp"
#include "syntax.hpp"
#include "parser.hpp"
#include "traversable_ast.hpp"
#include "analyzer.hpp"
#include "analyzed_abstract_syntax_tree.hpp"
#include "translator.hpp"
#include "import_system.hpp"

#include "debug_macro_definitions.hpp"

/* KoberiC class, which holds instances of all modules, initializes them, */
/* makes calls to member functions of these modules and ensures           */
/* input file is compiled                                                 */

class KoberiC {
    
    std::vector<token> _tokens;
    
    ImportSystem _importSystem;
    Tokenizer _tokenizer;
    Parser _parser;
    TraversableAbstractSyntaxTree _ast;
    AnalyzedAbstractSyntaxTree _aast;
    Analyzer _analyzer;
    Translator _translator;
    
    
    std::string _filename;
    
    std::vector<std::string> files;
    
    void parseInputFileName(const std::string & filename);
    
    void handleImports(const std::string & file);
    void tokenize(const std::string & filename);
    void parse();
    void analyze();
    void translate();
    
public:
    
    KoberiC();
    
    void compile(const std::string & filename);
    
    void test();
    
};

#endif /* koberi_c_hpp */
