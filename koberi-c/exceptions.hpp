//
//  exceptions.hpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#ifndef exceptions_hpp
#define exceptions_hpp

#include <stdio.h>
#include <iostream>

class unexpected_token : public std::exception {
    
    char _token;
    
public:
    
    const char * what();
    
    unexpected_token(char tok);
    
};

class missing_token : public std::exception {
    
    char _token;
    
public:
    
    const char * what();
    
    missing_token(char tok);
    
};

class invalid_operator : public std::exception {
    
    std::string _operator;
    
public:
    
    const char * what();
    
    invalid_operator(std::string & op);
    
};

class file_not_opened : public std::exception {
    
    std::string _filename;
    
public:
    
    const char * what();
    
    file_not_opened(const std::string & filename);
    file_not_opened(const char * filename);
    
};

class invalid_declaration : public std::exception {
  
    std::string _message;
    
public:
    
    const char * what();
    
    invalid_declaration(const std::string & message);
    invalid_declaration(const char * message);
    
};

class undeclared_function_call : public std::exception {
    
    std::string _message;
    
public:
    
    const char * what();
    undeclared_function_call(const std::string & message);
    
};

class bad_type : public std::exception {

    std::string _message;
    
public:
    
    const char * what();
    bad_type(const std::string & message);
    bad_type(const char * message);
    
};

#endif /* exceptions_hpp */
