//
//  ast_node.hpp
//  koberi-c
//
//  Created by Filip Peterek on 02/04/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#ifndef ast_node_hpp
#define ast_node_hpp

#include <stdio.h>
#include <functional>
#include <vector>
#include <unordered_map>

#include "parameter.hpp"

/* C++11 has ways of checking the type of a variable at runtime, but I'm going to use an enum instead */
enum class NodeType {
    
    Scope,
    FunCall,
    Class,
    Declaration,
    Function,
    Construct,
    Variable,
    Literal,
    Attribute,
    None /* Used for construction of ASTNode, which should never be constructed and None should never be used */
    
};

struct ASTScope;

/* This should be an abstract class, but to create an abstract class (or a struct, in this case), */
/* I'd need to create a virtual method                                                            */
/* But there's no place for methods here, so I'll make this instantiable                          */
/* Avoid using it if possible                                                                     */
struct ASTNode {
    
    ASTScope * parentScope; /* Can't use a reference because the global scope needs to point at 0 */
    NodeType nodeType;
    
};

/* A scope ({} in C) */
struct ASTScope : public ASTNode {
  
    std::vector<ASTNode*> childNodes;
    /* Format: Name - Data Type */
    /* Keeps track of variables for the current scope                                               */
    /* To access variables from parent scope, access parentScope.vars (apply recursively if needed) */
    std::unordered_map<std::string, std::string> vars;
    
    /* Pass in an ASTNode, not an ASTScope so the constructor isn't treated as a copy-constructor */
    ASTScope(ASTScope * parent);
    
    ~ASTScope();
    
    
};

/* A function call (operators count too) */
/* It will also be used for storing literals used in language constructs (while 1) */
struct ASTFunCall : public ASTNode {
    
    std::string function;
    std::vector<ASTNode *> parameters;
    
    ASTFunCall(ASTScope * parent, const std::string & name, const std::vector<ASTNode *> & params);
    ASTFunCall(const ASTFunCall & orig);
    ~ASTFunCall();
    
    static ASTFunCall * createFunCall(ASTScope * parentScope,
                                      const std::string & name,
                                      const std::vector<ASTNode *> & params);
    
};

/* Used to pass variables as funcall values                                    */
/* Shouldn't be used anywhere outside ASTFunCall parameters                    */
/* ParentScope holds pointer to the scope from which the variable was accessed */
struct ASTVariable : public ASTNode {
    
    std::string name;
    
    ASTVariable(const std::string & name, ASTScope * parentScope);
    
    static ASTVariable * createVariable(const std::string & variableName, ASTScope * parentScope);
    
};

/* Used to indicate access to object attributes */
struct ASTAttribute : public ASTNode {
    
    /* Just indicates how variables are accessed                                         */
    /* [object attributeObject number] translates to { object, attributeObject, number } */
    /* Which translates to object.attributeObject.number                                 */
    std::vector<std::string> accessOrder;
    
    ASTAttribute(const std::vector<std::string> & accessOrder);
    
};

/* Used to pass literals as funcall values                  */
/* Shouldn't be used anywhere outside ASTFunCall parameters */
struct ASTLiteral : public ASTNode {

    std::string type;
    std::string value;
    
    ASTLiteral(const parameter & literal);
    ASTLiteral(const std::string & type, const std::string & value);
    
    static ASTLiteral * createLiteral(const std::string & type,
                                      const std::string & value);

};

/* The Function node is essentially just a wrapper for a named scope with parameters (function)     */
/* The global scope will point forward to function scopes, but nothing will point to them backwards */
/* Function nodes are skipped and child nodes point to global scope as their parent                 */
struct ASTFunction : public ASTScope {
    
    std::string name;
    std::string type;
    std::vector<parameter> parameters;
    
    ASTFunction(ASTScope * parent,
                const std::string & functionName,
                const std::string & returnType,
                const std::vector<parameter> & params);
    
};

/* Similar functionality as ASTFunction, but it's used for language constructs such as if/while... */

struct ASTConstruct : public ASTScope {
    
    std::string construct;
    ASTNode * condition;
    
    ASTConstruct(ASTScope * parent,
                 const std::string & construct,
                 ASTNode * condition);
    
    ~ASTConstruct();
    
};

/* Variable declaration */
struct ASTDeclaration : public ASTNode {
    
    std::string type;
    std::string name;
    ASTNode * value;
    
    ASTDeclaration(ASTScope * parent,
                   const std::string & paramType,
                   const std::string & paramName,
                   ASTNode * paramValue);
    
};

#endif /* ast_node_hpp */
