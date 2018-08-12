//
//  Analyzer.hpp
//  koberi-c
//
//  Created by Filip Peterek on 30/10/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#ifndef Analyzer_hpp
#define Analyzer_hpp

#include "analyzed_abstract_syntax_tree.hpp"
#include "aast_node.hpp"
#include "traversable_ast.hpp"
#include "expressions.hpp"
#include "exceptions.hpp"

/* Analyzer class which iterates over AST nodes and analyzes them        */
/* Analyzer handles name mangling, type inferrence, type checking etc... */
/* Analyzer creates an AnalyzedAbstractSyntaxTree                        */

class Analyzer {
    
    TraversableAbstractSyntaxTree & _ast;
    AnalyzedAbstractSyntaxTree & _aast;
    
    ASTScope * _currentScope = nullptr;
    
    /* Used to store name of current function to make error messages slightly more useful */
    std::string _functionName;
    std::string _functionType;
    
    /* Concatenates '(', function type, function name and ')' */
    std::string currentFunction();
    
    /* Stores all currently declared variables so destructors can be called on all declared variables. */
    std::vector<ASTDeclaration *> _declarations;
    
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
    
    /* Creates a call to a virtual function -> function called via a v-table */
    AASTNode * virtualFunCall(const AASTValue & object, const std::string & funName,
                              const std::string & type, const std::vector<AASTNode *> & params);
    
    AASTOperator * analyzeOperator(const std::string & op, std::vector<AASTNode *> & params);
    AASTOperator * analyzePrint(std::vector<AASTNode *> & parameters);
    
    /* Analyzes return operator - checks if type matches function return type */
    /* Casts returned value to function's return type if possible             */
    AASTScope * analyzeReturn(std::vector<AASTNode *> & parameters);
    
    /* Analyzes inline C */
    AASTOperator * inlineC(std::vector<AASTNode *> & parameters, ASTFunCall & fcall);
    
    /* Casts objects to their superclasses/inheriting classes and int <-> num */
    AASTNode * cast(AASTNode * valueToCast, const std::string & type);
    
    /* New operator, which creates a new object on heap */
    AASTOperator * newObject(const std::string & type);
    
    /* Deletes an object by calling destructor and deallocating memory              */
    /* Returns a scope which calls object's destructor, frees it's allocated memory */
    /* and sets object pointer to NULL                                              */
    /* Not all steps are always possible when deleting objects                      */
    /* deleteObjects() only handles these steps if they're possible to perform      */
    AASTScope * deleteObject(AASTNode * object);
    
    /* Copies the value of an object into a variable while preserving vtable pointers   */
    /* Vtable pointers need to be preserved in case someone assigns a value of object's */
    /* superclass instance while the object was passed polymorphically as an instance   */
    /* of it's superclass                                                               */
    /* Copies contents of an object using memcpy                                        */
    AASTFuncall * copyObject(AASTNode * lvalue, AASTNode * rvalue);
    
    /* Checks if value is assignable (variable), throws exception on values which are */
    /* unassignable, eg. literals, function calls                                     */
    void checkIsAssignable(AASTNode * value);
    
    /* Analyzes a function call parameter */
    AASTNode * getFuncallParameter(ASTNode * node);
    
    /* Analyzes a scope ( {...} ) */
    AASTScope * analyzeScope(std::vector<ASTNode *> scopeNodes);
    
    /* Calls constructors on all objects with constructors from the current scope */
    std::vector<AASTFuncall *> destructScopedObjects(std::vector<ASTNode *> scopeNodes);
    
    /* Returns a destructor call for object passed as a parameter */
    AASTFuncall * getDestructor(AASTNode * object);
    
    /* Analyzes a single node found inside function bodies (construct, declaration, funcall) */
    AASTNode * analyzeFunctionNode(ASTNode * node);
    
    /* Analyzes constructs */
    AASTConstruct * analyzeConstruct(ASTConstruct & construct);
    
    /* If, else if, while, dowhile are syntactically the same, only the keyword is different */
    AASTConstruct * analyzeIfWhile(ASTConstruct & construct);
    /* Else if different because it doesn't have a condition */
    AASTConstruct * analyzeElse(ASTConstruct & construct);
    
    /* Analyzes variable declarations, deduces type if possible */
    AASTDeclaration * analyzeDeclaration(ASTDeclaration & declaration);
    
    AASTFuncall * getInitializer(ASTInitializer & initializer);
    
    AASTValue analyzeMemberAccess(ASTMemberAccess & attribute);
    /* Checks if nth parameter of member access has attribute n+1 and returns type of member access operator */
    std::string checkAttributesAndReturnType(parameter & var, std::vector<ASTNode*> & attributes, unsigned int iter = 1);
    
    /* Finds type of variable and returns variable name and type as parameter */
    parameter getVariable(ASTVariable & variable);
    
    /* Checks if parameter is an existing class, throws invalid_attribute_access exception if parameter isn't a class */
    void checkIsClass(std::string & className);
    
    /* Checks if id is a valid identifier for variable names */
    void checkIdIsValid(const std::string & id);
    
    /* Dereferences pointers using C * operator */
    AASTNode * dereference(const std::string & param);
    
    /* Check if type is a pointer type */
    bool isPointer(const std::string & type);
    
    
public:
    
    Analyzer(TraversableAbstractSyntaxTree & ast, AnalyzedAbstractSyntaxTree & aast);
    
    void analyze();
    
};

#endif /* Analyzer_hpp */
