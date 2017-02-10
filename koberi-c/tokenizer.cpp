//
//  tokenizer.cpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#include "tokenizer.hpp"


Tokenizer::Tokenizer(std::vector<token> & vectorRef) : _tokens(vectorRef) /* Call the reference constructor */ {
    
}

void Tokenizer::openFile(const std::string & filename) {
    
    _input.open(filename);
    if (not _input) { throw file_not_opened(filename); }
    
    // _filename = filename; /* Pointless now, but it might be necessary in the future */
    
}

void Tokenizer::readLine() {
    
    std::getline(_input, _line);
    _lineLen = _line.length();
    
}

void Tokenizer::identifierCheck() {
    
    std::string str(1, _line[_iter]); /* std::string constructor(repeat: int, character: char) */
    ++_iter;
    
    while ( syntax::isValidIdChar(_line[_iter]) or syntax::isNum(_line[_iter]) ) {
        
        str += std::string(1, _line[_iter]); /* std::string constructor(repeat: int, character: char) */
        ++_iter;
        
    }
    
    _tokens.emplace_back(tokType::id, str);
    
}

void Tokenizer::operatorCheck() {
    
    std::string str(1, _line[_iter]); /* std::string constructor(repeat: int, character: char) */
    ++_iter;
    
    while ( syntax::isOperatorChar(_line[_iter]) ) {
        
        str += std::string(1, _line[_iter]); /* std::string constructor(repeat: int, character: char) */
        ++_iter;
        
    }
    
    if ( syntax::isOperator(str) ) {
        
        _tokens.emplace_back(tokType::id, str);
        
    }
    else {
        
        throw invalid_operator(str);
        
    }
    
}

void Tokenizer::numberLiteral() {
    
    std::string str(1, _line[_iter]); /* std::string constructor(repeat: int, character: char) */
    ++_iter;
    
    bool isNum = false;
    
    while ( syntax::isNum(_line[_iter]) or (( _line[_iter] == '.' ) and not isNum) ) {
        
        if (_line[_iter] == '.' and isNum == false) { isNum = true; }
        str += std::string(1, _line[_iter]); /* std::string constructor(repeat: int, character: char) */
        ++_iter;
        
    }
    
    if (str.back() == '.') { throw unexpected_token('.'); }
    
    if (isNum) {
        _tokens.emplace_back(tokType::numLit, str);
    } else {
        _tokens.emplace_back(tokType::intLit, str);
    }
    
}

void Tokenizer::strLiteral() {
    
    std::string str;
    bool isEscape = false;
    
    ++_iter; /* First " character */
    
    while ( _line[_iter] != '"' or isEscape ) {
        
        if ( _line[_iter] == '\\' ) {
            
            isEscape = not isEscape;
            
            str += "\\";
            ++_iter;
            if (_iter >= _lineLen) { throw missing_token('"'); }
            
        }
        else {
            
            isEscape = false;
            
            str += std::string(1, _line[_iter]); /* std::string constructor(repeat: int, character: char) */
            ++_iter;
            if (_iter >= _lineLen) { throw missing_token('"'); }
            
        }
        
    }
    
    ++_iter; /* Last " character */
    _tokens.emplace_back(tokType::strLit, str);
    
}

void Tokenizer::parseLine() {
    
    for (_iter = 0; _iter < _lineLen; /* Don't increment here, iter is incremented inside loop */) {
        
        /* Check for single line comment, if found, break out of loop  */
        /* The content of a comment is none of the compiler's bussines */
        
        if ( _line[_iter] == ';' ) {  break;  }
        
        /* Ignore whitespace, it's not important anymore, just increment iterator */
        
        else if ( syntax::isWhiteSpace(_line[_iter]) ) {  ++_iter;  }
        
        /* Check for brackets */
        
        else if ( _line[_iter] == '[' ) {
            
            _tokens.emplace_back( tokType::openingBra, std::string("[") );
            ++_iter;
            
        }
        else if ( _line[_iter] == ']' ) {
            
            _tokens.emplace_back( tokType::closingBra, std::string("]") );
            ++_iter;
            
        }
        
        /* Check for parenthesis */
        
        else if ( _line[_iter] == '(' ) {
            
            _tokens.emplace_back( tokType::openingPar, std::string("(") );
            ++_iter;
            
        }
        else if ( _line[_iter] == ')' ) {
            
            _tokens.emplace_back(tokType::closingPar, std::string(")") );
            ++_iter;
            
        }
        
        /* Check for an identifier */
        
        else if ( syntax::isValidIdChar(_line[_iter]) ) {  identifierCheck();  }
        
        /* Check for a num or flt literal */
        
        else if ( syntax::isNum(_line[_iter]) or _line[_iter] == '-' ) {  numberLiteral();  }
        
        /* Check for operator */
        
        else if ( syntax::isOperatorChar(_line[_iter]) ) {  operatorCheck();  }
        
        /* Check for a str literal */
        
        else if ( _line[_iter] == '"' ) {  strLiteral();  }
        
        else {  throw unexpected_token(_line[_iter]);  }
        
    } /* for */
    
} /* parseLine() */

void Tokenizer::tokenizeFile(const std::string & filename) {
    
    openFile(filename);
    
    while ( not _input.eof() ) {
        
        readLine();
        
#ifdef PRINT_LINE
        print(_line);
#endif
        
        parseLine();
        
    }
    
}
