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
#include <string>
#include <sstream>

/* TODO: Rewrite exceptions with unmodifiable error messages to use a static string to store their messages */

class unexpected_token : public std::exception {
    
    std::string _message;
    
public:
    
    const char * what() const throw();
    
    unexpected_token(char tok);
    unexpected_token(const std::string & tok);
    
};

class missing_token : public std::exception {
    
    std::string _message;
    
public:
    
    const char * what() const throw();
    
    missing_token(char tok);
    
};

class invalid_operator : public std::exception {
    
    std::string _message;
    
public:
    
    const char * what() const throw();
    
    invalid_operator(std::string & op);
    
};

class file_not_opened : public std::exception {
    
    std::string _message;
    
public:
    
    const char * what() const throw();
    
    file_not_opened(const std::string & filename);
    file_not_opened(const std::string && filename);
    
};

class file_not_created : public std::exception {
    
    std::string _message;
    
public:
    
    const char * what() const throw();
    
    file_not_created(const std::string & filename);
    
};

class invalid_declaration : public std::exception {
  
    std::string _message;
    
public:
    
    const char * what() const throw();
    
    invalid_declaration(const std::string & message);
    invalid_declaration(const std::string && message);
    
};

class undeclared_function_call : public std::exception {
    
    std::string _message;
    
public:
    
    const char * what() const throw();
    undeclared_function_call(const std::string & function);
    undeclared_function_call(const std::string & method, const std::string & className);
    
};

class undefined_variable : public std::exception {
    
    std::string _message;
    
public:
    
    const char * what() const throw();
    undefined_variable(const std::string & var_name);
    
};

class bad_type : public std::exception {

    std::string _message;
    
public:
    
    const char * what() const throw();
    bad_type(const std::string & message);
    bad_type(const std::string && message);
    
};

class invalid_syntax : public std::exception {
    
    std::string _message;
    
public:
    
    const char * what() const throw();
    invalid_syntax(const std::string & message);
    invalid_syntax(const std::string && message);
    
};

class undefined_class : public std::exception {
    
    std::string _message;
    
public:
    
    const char * what() const throw();
    undefined_class(const std::string & className);
    undefined_class(const std::string && className);
    
};

class redefinition_of_class : public std::exception {
    
    std::string _message;
    
public:
    
    const char * what() const throw();
    redefinition_of_class(const std::string & className);
    
};

class not_a_class : public std::exception {
    
    std::string _message;
    
public:
    
    const char * what() const throw();
    not_a_class(const std::string & id);
    not_a_class(const std::string && id);
    
};

class redefinition_of_attribute : public std::exception {
    
    std::string _message;
    
public:
    
    redefinition_of_attribute(const std::string && attributeName, const std::string && className);
    redefinition_of_attribute(const std::string & attributeName, const std::string & className);

    const char * what() const throw();
    
};

class redefinition_of_variable : public std::exception {
    
    std::string _message;
    
public:
    
    redefinition_of_variable(const std::string & varName);
    
    const char * what() const throw();
    
};

class redefinition_of_function {
    
    std::string _message;
    
public:
    
    redefinition_of_function(const std::string & functionName);
    redefinition_of_function(const std::string & functionName, const std::string & className);
    
    const char * what () const throw();
    
};

class no_such_member : public std::exception {
    
    std::string _message;
    
public:
    
    no_such_member(const std::string && memberName, const std::string && className);
    no_such_member(const std::string & memberName, const std::string & className);
    
    const char * what() const throw();
    
};

class exiting_global_scope : public std::exception {

    std::string _message;
    
public:
    
    exiting_global_scope();
    
    const char * what() const throw();

};

class wrong_scope : public std::exception {

    std::string _message;
    
public:
    
    wrong_scope(const std::string & message);
    
    const char * what() const throw();
    
};

class not_a_scope : public std::exception {
    
    static const std::string message;
    
public:
    const char * what() const throw();
    
};

class compiler_error : public std::exception {
    
    const std::string message;
    
public:
    
    compiler_error(const std::string & message);
    const char * what() const throw();
    
};

class invalid_main : public std::exception {
    
    const std::string message;
    
public:
    
    invalid_main();
    const char * what() const throw();
    
};

class missing_main : public std::exception {
    
    const std::string message;
    
public:
    
    missing_main();
    const char * what() const throw();
    
};

class invalid_statement : public std::exception {
    
    const std::string message;
    
public:
    
    invalid_statement(const std::string & functionName);
    const char * what() const throw();
    
};

class type_mismatch : public std::exception {
    
    const std::string message;
    
public:
    
    type_mismatch(const std::string & msg);
    const char * what() const throw();
    
};

class invalid_parameter : public std::exception {
    
    const std::string message;
    
public:
    
    invalid_parameter(const std::string & functionName, const std::string & funcall, const std::string & parameter);
    const char * what() const throw();
    
};

class invalid_call : public std::exception {
    
    const std::string message;
    
public:
    
    invalid_call(const std::string & funcall, const std::string & functionName, const std::string & msg);
    const char * what() const throw();
    
};

class invalid_attribute_access : public std::exception {
    
    const std::string message;
    
public:
    
    invalid_attribute_access(const std::string & functionName, const std::string & msg);
    invalid_attribute_access(const std::string & msg);
    const char * what() const throw();
    
};

class invalid_cast : public std::exception {
    
    const std::string _message;
    
public:
    
    invalid_cast(const std::string & origType, const std::string & newType);
    const char * what() const throw();
    
};

class invalid_char_literal : public std::exception {
  
    const std::string _message;
    
public:
    
    invalid_char_literal(const std::string & literal);
    const char * what() const throw();
    
};

#endif /* exceptions_hpp */
