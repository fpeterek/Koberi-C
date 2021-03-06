//
//  ast_node.hpp
//  koberi-c
//
//  Created by Filip Peterek on 02/04/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#ifndef ast_node_hpp
#define ast_node_hpp

#include <string>
#include <vector>
#include <unordered_map>

#include "parameter.hpp"
#include "exceptions.hpp"

/* AbstractSyntaxTree nodes                         */
/* Used to store parsed Kobeři-C code inside a tree */

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
    MemberAccess,
    Initializer,
    None /* Used for construction of ASTNode, which should never be constructed and None should never be used */
    
};

struct ASTScope;


struct ASTNode {
    
    ASTScope * parentScope; /* Can't use a reference because the global scope needs to point at 0 */
    NodeType nodeType;
    
    virtual ~ASTNode() = 0;
    
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

struct ASTMemberAccess;

/* A function call (used for member functions and operators, too)                  */
/* It will also be used for storing literals used in language constructs (while 1) */
struct ASTFunCall : public ASTNode {
    
    std::string function;
    std::vector<ASTNode *> parameters;
    
    /* Used to point to an object on which a member function is called */
    ASTMemberAccess * object;
    
    ASTFunCall(ASTScope * parent,
               const std::string & name,
               const std::vector<ASTNode *> & params,
               ASTMemberAccess * object = nullptr);
    ASTFunCall(const ASTFunCall & orig);
    ~ASTFunCall();
    
    static ASTFunCall * createFunCall(ASTScope * parentScope,
                                      const std::string & name,
                                      const std::vector<ASTNode *> & params,
                                      ASTMemberAccess * object = nullptr);
    
};

/* Used to pass variables as funcall values                                    */
/* Shouldn't be used anywhere outside ASTFunCall parameters                    */
/* ParentScope holds pointer to the scope from which the variable was accessed */
struct ASTVariable : public ASTNode {
    
    std::string name;
    
    ASTVariable(const std::string & name, ASTScope * parentScope);
    
    static ASTVariable * createVariable(const std::string & variableName, ASTScope * parentScope);
    
};

/* Used to indicate access to object members */
struct ASTMemberAccess : public ASTNode {
    
private:
    
    void check();
    
public:
    
    /* Just indicates how members are accessed                                                  */
    /* [object attributeObject number] translates to { object, attributeObject, number }        */
    /* Which translates to object.attributeObject.number                                        */
    /* [(get_object 2 3 4) attribute number] translates to get_object(2, 3, 4).attribute.number */
    std::vector<ASTNode*> accessOrder;
    
    ASTMemberAccess(const std::vector<ASTNode*> & accessOrder, ASTScope * parentScope);
    ASTMemberAccess(const ASTMemberAccess & orig);
    ~ASTMemberAccess();
    
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
    std::string className;
    std::vector<parameter> parameters;
    
    ASTFunction(ASTScope * parent,
                const std::string & functionName,
                const std::string & returnType,
                const std::vector<parameter> & params,
                const std::string & className);
    
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

/* Call to object initializer */
struct ASTInitializer : public ASTNode {
    
    std::string name;
    
    ASTInitializer(ASTScope * parent,
                   const std::string & varName);
    
};

#endif /* ast_node_hpp */
