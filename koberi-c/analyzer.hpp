//
//  Analyzer.hpp
//  koberi-c
//
//  Created by Filip Peterek on 30/10/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#ifndef Analyzer_hpp
#define Analyzer_hpp

#include <stdio.h>
#include <iostream>
#include <tuple>
#include <functional>

#include "analyzed_abstract_syntax_tree.hpp"
#include "aast_node.hpp"
#include "traversable_ast.hpp"
#include "expressions.hpp"
#include "exceptions.hpp"

class Analyzer {
    
    TraversableAbstractSyntaxTree & _ast;
    AnalyzedAbstractSyntaxTree & _aast;
    
    /* Used to store name of current function to make error messages slightly more useful */
    std::string _functionName;
    
    void analyzeClasses();
    void analyzeGlobalVars();
    
    /* Iterates over nodes and calls analyzeFunction() on each function */
    void analyzeFunctions();
    
    /* Checks whether main function exists and follows Kobeři-C rules for (int main ()) */
    void kobericMainCheck();
    
    /* Analyzes a function, checks it for mistakes, prepares it for translation to C */
    void analyzeFunction(ASTFunction & function);
    
    /* Analyzes a function call, mangles the name */
    AASTNode * analyzeFunCall(ASTFunCall & funcall);
    
    AASTOperator * analyzeOperator(const std::string & op, const std::vector<AASTNode *> & params);
    AASTOperator * analyzePrint(std::vector<AASTNode *> & parameters);
    AASTOperator * inlineC(std::vector<AASTNode *> & parameters, ASTFunCall & fcall);
    
    /* Casts objects to their superclasses/inheriting classes and int <-> num */
    AASTNode * cast(AASTNode * valueToCast, const std::string & type);
    
    /* New operator, which creates a new object on heap */
    AASTOperator * newObject(const std::string & type);
    
    /* Deletes an object by calling destructor and deallocating memory                        */
    /* Funcall is destructor, first operator is free and second operator sets pointer to NULL */
    // std::tuple<AASTFuncall*, AASTOperator, AASTOperator> deleteObject(parameter & object, const bool isFuncall);
    AASTScope * deleteObject(AASTNode * object);
    
    AASTNode * getFuncallParameter(ASTNode * node);
    
    /* Analyzes a scope ( {...} ) */
    AASTScope * analyzeScope(std::vector<ASTNode *> scopeNodes);
    
    /* Calls constructors on all objects with constructors from the current scope */
    std::vector<AASTFuncall *> destructScopedObjects(std::vector<ASTNode *> scopeNodes);
    
    AASTFuncall * getDestructor(AASTNode * object);
    
    /* Analyzes a single node found inside function bodies (construct, declaration, funcall) */
    AASTNode * analyzeFunctionNode(ASTNode * node);
    
    /* Analyzes constructs */
    AASTConstruct * analyzeConstruct(ASTConstruct & construct);
    
    /* If, else if, while are syntactically the same, only the keyword is different */
    AASTConstruct * analyzeIfWhile(ASTConstruct & construct);
    AASTConstruct * analyzeElse(ASTConstruct & construct);
    
    AASTDeclaration * analyzeDeclaration(ASTDeclaration & declaration);
    
    AASTValue analyzeMemberAccess(ASTMemberAccess & attribute);
    std::string checkAttributesAndReturnType(parameter & var, std::vector<ASTNode*> & attributes, unsigned int iter = 1);
    parameter getVariable(ASTVariable & variable);
    
    /* Checks if parameter is an existing class, throws invalid_attribute_access exception if parameter isn't a class */
    void checkIsClass(std::string & className);
    
    /* Checks if id is a valid identifier for variable names */
    void checkIdIsValid(const std::string & id);
    
    AASTNode * dereference(const std::string & param);
    
    bool isPointer(const std::string & type);
    
    
public:
    
    Analyzer(TraversableAbstractSyntaxTree & ast, AnalyzedAbstractSyntaxTree & aast);
    
    void analyze();
    
};

#endif /* Analyzer_hpp */
