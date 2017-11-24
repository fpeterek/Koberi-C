//
//  traversable_ast.hpp
//  koberi-c
//
//  Created by Filip Peterek on 14/05/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#ifndef traversable_ast_hpp
#define traversable_ast_hpp

#include <stdio.h>

#include "ast.hpp"

/* Tuples are a bit ugly in C++, structs look better so I'm using a struct       */
/* Return type for getMethodReturnType(), which holds the methods return type    */
/* and name of the class it belongs to                                           */
/* Used to mangle names properly and cast objects properly to their superclasses */

struct method {
    
    std::string type;
    std::string className;
    
};

/* TraversableAbstractSyntaxTree class extends AbstractSyntaxTree     */
/* and provides an interface which allows user to work with AST nodes */

class TraversableAbstractSyntaxTree : public AbstractSyntaxTree {
    
    /* Recursively searches whether a variable exists and returns */
    /* it's type or throws an exception                           */
    std::string getVarTypeRecursive(const std::string & varName, ASTScope * scope);
    
public:
    
    /* Used to enter or leave a certain scope */
    void enterScope(size_t scopeIndex);
    void exitScope();
    
    /* Used to work with nodes of current scope */
    ASTNode & getNode(size_t nodeIndex);
    ASTNode * getNodePtr(size_t nodeIndex);
    size_t getNodeCount();
    
    /* Provides access to current scope - scope which is currently being worked with */
    ASTScope & getCurrentScope();
    ASTScope * getCurrentScopePtr();
    
    /* Provides access to global scope */
    ASTScope & getGlobalScope();
    ASTScope * getGlobalScopePtr();
    
    /* Searches for a variable in current scope, internally calls getVarTypeRecursive() */
    std::string getVarType(const std::string & varName);
    /* Searches for a variable in provided scope, internally calls getVarTypeRecursive() */
    std::string getVarType(const std::string & varName, ASTScope * scope);
    
    /* Returns the return type of a function, throws if function can't be found */
    std::string getFunctionReturnType(const std::string & funName);
    
    /* Returns classes as an unordered structure */
    const std::unordered_map<std::string, _class> & getClasses();
    /* Returns class names (keys) in the order in which the classes were declared */
    const std::vector<std::string> & getClassOrder();
    
    const _class & getClass(const std::string & className);
    
    /* Checks if param is an existing data type, including primitive types and classes */
    bool isDataType(const std::string & param);
    /* Checks if param is an existing class  */
    bool isClass(const std::string & param);
    
    /* Recursively checks if a class or it's superclasses inherit from a specified class */
    bool hasSuperclass(const std::string & className, const std::string & superClass);
    /* Recursively checks if a class or it's superclasses define specified method */
    bool hasMethod(const std::string & methodName, const std::string & className);
    /* Recursively checks if a class or it's superclasses define a destructor */
    bool hasDestructor(const std::string & className);
    
    /* Returns return type of a member function as well as the class said function belongs to */
    method getMethodReturnType(const std::string & methodName, const std::string & className);
    
};

#endif /* traversable_ast_hpp */
