//
//  exceptions.cpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#include "exceptions.hpp"


unexpected_token::unexpected_token(char tok) {
    
    _message = "Unexpected token: ";
    // _message += std::to_string(tok);
    _message += { tok, 0 };
    
}

unexpected_token::unexpected_token(const std::string & tok) {
    
    _message = "Unexpected token: " + tok;
    
}

const char * unexpected_token::what() const throw() {
    
    return _message.c_str();
    
}


missing_token::missing_token(char tok) {
    
    _message = "Missing token ";
    // _message += std::to_string(tok);
    _message += { tok, 0 };

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

const char * file_not_created::what() const throw() {
    
    return _message.c_str();
    
}

file_not_created::file_not_created(const std::string & filename) {
    
    _message = "File " + filename + " could not be created. ";
    
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

undeclared_function_call::undeclared_function_call(const std::string & method, const std::string & className) {
    
    _message = "Call to undeclared member function " + method + " on object of type " + className;
    
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

redefinition_of_class::redefinition_of_class(const std::string & className) {
    
    _message = "Redefinition of class " + className;
    
}

const char * redefinition_of_class::what() const throw() {
    
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

redefinition_of_variable::redefinition_of_variable(const std::string & varName) {

    _message = "Redefinition of variable " + varName;
    
}

const char * redefinition_of_variable::what() const throw() {
    
    return _message.c_str();
    
}

redefinition_of_function::redefinition_of_function(const std::string & functionName) {
    
    _message = "Redefinition of function " + functionName;
    
}

redefinition_of_function::redefinition_of_function(const std::string & functionName, const std::string & className) {
    
    _message = "Redefinition of member function " + functionName + " of class " + className;
    
}

const char * redefinition_of_function::what () const throw() {
    
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

exiting_global_scope::exiting_global_scope() {
    
    std::stringstream message;
    message << "Error: Attempting to exit global scope. "
            << "This error message should never find it's way towards the end-user. "
            << "If you happen to be an end-user and you see this message, something has gone wrong.";
    _message = message.str();
    
}

const char * exiting_global_scope::what() const throw() {
    
    return _message.c_str();
    
}

wrong_scope::wrong_scope(const std::string & message) {

    _message = message;
    
}

const char * wrong_scope::what() const throw() {

    return _message.c_str();
    
}

const std::string not_a_scope::message = "Node can not be casted to a scope node because node is not a scope. ";

const char * not_a_scope::what() const throw() {
    
    return not_a_scope::message.c_str();
    
}

compiler_error::compiler_error(const std::string & newMessage)
    : message(newMessage) {
        
}

const char * compiler_error::what() const throw() {
    
    return message.c_str();
    
}

invalid_main::invalid_main() : message("Error: (main ()) must return int. ") { }

const char * invalid_main::what() const throw() {
    
    return message.c_str();
    
}

missing_main::missing_main() : message("Error: Missing (int main ()). ") { }

const char * missing_main::what() const throw() {
    
    return message.c_str();
    
}

invalid_statement::invalid_statement(const std::string & functionName) :
    message("Error: Invalid statment in function " + functionName + ". ") { }

const char * invalid_statement::what() const throw() {
    
    return message.c_str();
    
}

type_mismatch::type_mismatch(const std::string & msg) : message(msg) { }

const char * type_mismatch::what() const throw() {
    
    return message.c_str();
    
}

/* Beautiful */
invalid_parameter::invalid_parameter(const std::string & functionName,
                                     const std::string & funcall,
                                     const std::string & parameter)
                                    : message("Invalid parameter " + parameter +
                                              " in call to " + funcall +
                                              " in function " + functionName) { }

const char * invalid_parameter::what() const throw() {
    
    return message.c_str();
    
}

/* Beautiful */
invalid_call::invalid_call(const std::string & funcall,
                           const std::string & functionName,
                           const std::string & msg)
                            : message("Invalid call to " + funcall +
                                      " in function " + functionName +
                                      ": " + msg) { }

const char * invalid_call::what() const throw() {
    
    return message.c_str();
    
}

invalid_attribute_access::invalid_attribute_access(const std::string & functionName,
                                                   const std::string & msg)
                                                    : message("Invalid attribute access in function "
                                                               + functionName + ". " + msg) {
    
}

invalid_attribute_access::invalid_attribute_access(const std::string & msg)
                                                 : message("Invalid attribute access: " + msg) {
    
}

const char * invalid_attribute_access::what() const throw() {
    
    return message.c_str();
    
}

invalid_cast::invalid_cast(const std::string & origType, const std::string & newType)
                            : _message("Cannot cast value of type "
                                       + origType + " to " + newType) {

}

const char * invalid_cast::what() const throw() {
    
    return _message.c_str();
    
}

invalid_char_literal::invalid_char_literal(const std::string & literal)
                                          : _message("Invalid character literal: " + literal) {
    
}

const char * invalid_char_literal::what() const throw() {
    
    return _message.c_str();
    
}

invalid_identifier::invalid_identifier(const std::string & identifier)
                                    : _message("Error: " + identifier + " is not a valid identifier") {
    
}

const char * invalid_identifier::what() const throw() {
    
    return _message.c_str();
    
}

type_deduction_error::type_deduction_error(const std::string & varName, const std::string & function) :
                    _message("Error: Type of variable " + varName
                             + " in function " + function
                             + " could not be automatically deduced.") {
    
}
const char * type_deduction_error::what() const throw() {
    
    return _message.c_str();
    
}



