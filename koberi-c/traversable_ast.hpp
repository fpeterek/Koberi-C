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
    
    bool isDataType(const std::string & param);
    
};

#endif /* traversable_ast_hpp */
