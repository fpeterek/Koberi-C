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
/* And name of the class it belongs to                                           */
/* Used to mangle names properly and cast objects properly to their superclasses */

struct method {
    
    std::string type;
    std::string className;
    
};

class TraversableAbstractSyntaxTree : public AbstractSyntaxTree {
    
    std::string getVarTypeRecursive(const std::string & varName, ASTScope * scope);
    
public:
    
    void enterScope(size_t scopeIndex);
    void exitScope();
    
    ASTNode & getNode(size_t nodeIndex);
    ASTNode * getNodePtr(size_t nodeIndex);
    size_t getNodeCount();
    
    ASTScope & getCurrentScope();
    ASTScope * getCurrentScopePtr();
    
    ASTScope & getGlobalScope();
    ASTScope * getGlobalScopePtr();
    
    std::string getVarType(const std::string & varName);
    std::string getVarType(const std::string & varName, ASTScope * scope);
    std::string getFunctionReturnType(const std::string & funName);
    
    const std::unordered_map<std::string, _class> & getClasses();
    const std::vector<std::string> & getClassOrder();
    
    const _class & getClass(const std::string & className);
    
    bool isDataType(const std::string & param);
    bool isClass(const std::string & param);
    
    bool hasSuperclass(const std::string & className, const std::string & superClass);
    bool hasMethod(const std::string & methodName, const std::string & className);
    bool hasDestructor(const std::string & className);
    
    method getMethodReturnType(const std::string & methodName, const std::string & className);
    
};

#endif /* traversable_ast_hpp */
