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
    
    
    /* This could be a char, but people might use software-generated code                */
    /* If someone manages to make this overflow, they don't deserve to use this compiler */
    unsigned short _indentLevel = 0;
    
    /* Used to store name of current function to make error messages slightly more useful */
    std::string _functionName;

    
    void main();
    
    /* Checks whether main function exists and follows Kobeři-C rules for (int main ()) */
    void kobericMainCheck();
    
    /* Translates a Kobeři-C function into a C function. */
    void translateFunction(ASTFunction & function);
    
    /* Translates a Kobeři-C function into a C function and returns it as a parameter */
    /* Type is the return type of the function and value is a C funcall as a string   */
    parameter translateFunCall(ASTFunCall & funcall);
    
    parameter translateOperator(std::string & op, std::vector<parameter> & params);
    parameter translatePrint(std::vector<parameter> & parameters);
    parameter inlineC(std::vector<parameter> & parameters);
    
    /* Casts objects to their superclasses/inheriting classes and int <-> num */
    parameter cast(const parameter & valueToCast, const std::string & type);
    
    /* Creates a new object on heap */
    parameter newObject(const std::string & type);
    
    /* Deletes an object by calling destructor and deallocating memory                      */
    /* If parameter is a scoped variable and not an rvalue returned by function set it to 0 */
    parameter deleteObject(parameter & object, const bool isFuncall);
    
    parameter getFuncallParameter(ASTNode * node, const bool derefCharPointers = true);
    
    /* Translates a scope ( {...} ) */
    void translateScope(std::vector<ASTNode *> scopeNodes);
    /* Calls constructors on all objects with constructors from the current scope */
    void destructScopedObjects(std::vector<ASTNode *> scopeNodes);
    
    std::string getDestructor(const parameter & object);
    
    /* Translates node found inside function bodies (constructs, declarations, funcalls) */
    void translateFunctionNode(ASTNode * node);
    
    /* Translates constructs and outputs them to _output */
    void translateConstruct(ASTConstruct & construct);
    
    /* If, else if, while are syntactically the same, only the keyword is different */
    std::string translateIfWhile(ASTConstruct & construct);
    std::string translateElse(ASTConstruct & construct);
    
    std::string translateDeclaration(ASTDeclaration & declaration);
    
    parameter translateMemberAccess(ASTMemberAccess & attribute);
    std::string checkAttributesAndReturnType(parameter & var, std::vector<ASTNode*> & attributes, unsigned int iter = 1);
    parameter getVariable(ASTVariable & variable);
    
    /* Checks if type is int. If type is int/uint, returns ll/ull respectively */
    /* Otherwise returns parameter                                             */
    std::string translateType(const std::string & type);
    
    /* Checks if parameter is an existing class, throws invalid_attribute_access exception if parameter isn't a class */
    void checkIsClass(std::string & className);
    
    /* Checks if id is a valid identifier for variable names */
    void checkIdIsValid(const std::string & id);
    
    /* Indents code */
    void indent();
    
    std::string dereference(const std::string & param);
    bool isPointer(const std::string & type);
    
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
    
    void test();
    
};

#endif /* translator_hpp */
