//
//  exceptions.cpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#include "exceptions.hpp"


unexpected_token::unexpected_token(char tok) {
    
    _message = "Unexpected token ";
    _message += std::to_string(tok);
    
}

const char * unexpected_token::what() const throw() {
    
    return _message.c_str();
    
}


missing_token::missing_token(char tok) {
    
    _message = "Missing token ";
    _message += std::to_string(tok);

}

const char * missing_token::what() const throw() {
    
    return _message.c_str();
    
}


invalid_operator::invalid_operator(std::string & op) {
    
    _message = "Invalid operator " + op;
    
}

const char * invalid_operator::what() const throw() {
    
    return _message.c_str();
    
}

file_not_opened::file_not_opened(const std::string & filename) {
    
    _message = "File " + filename + " could not be opened. ";
    
}

file_not_opened::file_not_opened(const std::string && filename) {
    
    _message = "File " + filename + " could not be opened. ";
    
}

const char * file_not_opened::what() const throw() {
    
    return _message.c_str();
    
}

const char * invalid_declaration::what() const throw() {
    return _message.c_str();
}

invalid_declaration::invalid_declaration(const std::string & message) {
    
    _message = message;

}

invalid_declaration::invalid_declaration(const std::string && message) {

    _message = message;

}

undeclared_function_call::undeclared_function_call(const std::string & message) {

    _message = "Call to undeclared function " + message;

}

const char * undeclared_function_call::what() const throw() {

    return _message.c_str();

}

bad_type::bad_type(const std::string & message) {

    _message = message;

}

bad_type::bad_type(const std::string && message) {

    _message = message;

}

const char * bad_type::what() const throw() {

    return _message.c_str();

}

undefined_variable::undefined_variable(const std::string & var_name) {

    _message = "Reference to undefined variable " + var_name;
    
}

const char * undefined_variable::what() const throw() {
    
    return _message.c_str();
    
}

invalid_syntax::invalid_syntax(const std::string & message) {
    
    _message = message;

}

invalid_syntax::invalid_syntax(const std::string && message) {
    
    _message = message;

}

const char * invalid_syntax::what() const throw() {
    
    return _message.c_str();

}

undefined_class::undefined_class(const std::string & className) {
    
    _message = "Inheriting from an undefined class " + className;

}

undefined_class::undefined_class(const std::string && className) {
    
    _message = "Inheriting from an undefined class " + className;
    
}

const char * undefined_class::what() const throw() {
    
    return _message.c_str();

}

not_a_class::not_a_class(const std::string & id) {
    
    _message = id + " is not a class";
    
}

not_a_class::not_a_class(const std::string && id) {
    
    _message = id + " is not a class";
    
}

const char * not_a_class::what() const throw() {
    
    return _message.c_str();
    
}

redefinition_of_attribute::redefinition_of_attribute(const std::string && attributeName, const std::string && className) {
    
    _message = "Redefinition of class attribute " + attributeName + " in class " + className;

}

redefinition_of_attribute::redefinition_of_attribute(const std::string & attributeName, const std::string & className) {
    
    _message = "Redefinition of class attribute " + attributeName + " in class " + className;
    
}

const char * redefinition_of_attribute::what() const throw() {
    
    return _message.c_str();
    
}

no_such_member::no_such_member(const std::string && memberName, const std::string && className) {
    
    _message = className + " has no member " + memberName;
    
}

no_such_member::no_such_member(const std::string & memberName, const std::string & className) {
    
    _message = className + " has no member " + memberName;
    
}

const char * no_such_member::what() const throw() {
    
    return _message.c_str();
    
}


