//
//  tokenizer.hpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#ifndef tokenizer_hpp
#define tokenizer_hpp

#include <vector>
#include <fstream>

#include "syntax.hpp"
#include "exceptions.hpp"
#include "token.hpp"

#include "debug_macro_definitions.hpp"

/* Tokenizer class, which handles both tokenization and lexical analysis */
/* Kobeři-C syntax is quite simple and code is easy to tokenize and analyze simultaneously */

class Tokenizer {
    
    std::vector<token> & _tokens; /* Reference to a vector of tokens */
    
    /* Input file stream */
    std::ifstream _input;
    
    /* Holds a single line read from input stream */
    std::string _line;
    
    /* Holds line length so _line.size() doesn't have to be called repeatedly */
    size_t _lineLen;
    /* Iterator used to iterate over _line chars */
    size_t _iter;
    
    /* Checks if token is a valid identifier */
    void identifierCheck();
    /* Checks if minus is used as an operator or if it's part of a literal */
    void minus();
    /* Lexes operators, checks if they are valid */
    void operatorCheck();
    /* Lexes numerical literals */
    void numberLiteral();
    /* Tokenizes and lexes char and string literals */
    void strLiteral(const char delimiter='"');
    /* Reads a single line from input stream */
    void readLine();
    /* Parses _line */
    void parseLine();
    
    void openFile(const std::string & filename);
    
public:
    
    Tokenizer(std::vector<token> & vectorRef);
    void tokenizeFile(const std::string & filename);
    
};


#endif /* tokenizer_hpp */
