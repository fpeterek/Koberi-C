//
//  exceptions.cpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#include "exceptions.hpp"


unexpected_token::unexpected_token(char tok) {
    _token = tok;
}

const char * unexpected_token::what() {
    std::string message = "Unexpected token ";
    message += std::to_string(_token);
    return message.c_str();
}


missing_token::missing_token(char tok) {
    _token = tok;
}

const char * missing_token::what() {
    std::string message = "Missing token ";
    message += std::to_string(_token);
    return message.c_str();
}


invalid_operator::invalid_operator(std::string & op) {
    _operator = op;
}

const char * invalid_operator::what() {
    std::string message = "Invalid operator " + _operator;
    return message.c_str();
}

file_not_opened::file_not_opened(const std::string & filename) {
    _filename = filename;
}

file_not_opened::file_not_opened(const char * filename) {
    _filename = filename;
}

const char * file_not_opened::what() {
    std::string message = "File " + _filename + " could not be opened. ";
    return message.c_str();
}

const char * invalid_declaration::what() {
    return _message.c_str();
}

invalid_declaration::invalid_declaration(const std::string & message) {
    _message = message;
}

invalid_declaration::invalid_declaration(const char * message) {
    _message = message;
}

undeclared_function_call::undeclared_function_call(const std::string & message) {
    _message = "Call to undeclared function " + message;
}

const char * undeclared_function_call::what() {
    return _message.c_str();
}

bad_type::bad_type(const std::string & message) {
    _message = message;
}

bad_type::bad_type(const char * message) {
    _message = message;
}

const char * bad_type::what() {
    return _message.c_str();
}
