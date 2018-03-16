//
//  aast_node.hpp
//  koberi-c
//
//  Created by Filip Peterek on 30/10/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#ifndef aast_node_hpp
#define aast_node_hpp

#include <iostream>
#include <sstream>
#include <vector>

#include "syntax.hpp"
#include "parameter.hpp"

#define INDENT "    " /* Use four spaces to indent */

/* Set of self-translating AnalyzedAST nodes     */
/* Nodes recursively translate themselves into C */

enum class AASTNodeType {
    
    Construct,
    Function,
    Scope,
    Class,
    Funcall,
    Value,
    Declaration,
    Operator,
    Cast
    
};

class AASTConstruct;
class AASTFunction;
class AASTScope;
class AASTClass;
class AASTFuncall;
class AASTValue;
class AASTDeclaration;
class AASTOperator;
class AASTCast;

/* Abstract base class */
class AASTNode {
    
    const AASTNodeType _nodeType;
    const std::string _type;
    
public:
    
    AASTNode(AASTNodeType nodeType, const std::string & dataType);
    virtual ~AASTNode();
    
    AASTNodeType nodeType() const;
    std::string type() const;
    
    virtual std::string value(int baseIndent = 0) const = 0;
    
};

/* A scope -> {} */
class AASTScope : public AASTNode {
    
    const std::vector<AASTNode *> _body;
    
public:
    
    AASTScope(const std::vector<AASTNode *> & body);
    ~AASTScope();
    std::string value(int baseIndent = 0) const;
    
};

/* Language constructs used for control flow -> while, dowhile, if/elif/else */
class AASTConstruct : public AASTNode {
    
    const std::string _construct;
    const AASTNode * _condition;
    const AASTScope * _body;
    
public:
    
    AASTConstruct(const std::string & construct, const AASTNode * condition, const AASTScope * body);
    ~AASTConstruct();
    
    std::string value(int baseIndent = 0) const;
    
};

/* Function and member function definitions */
class AASTFunction : public AASTNode {
    
    const std::vector<AASTDeclaration> _parameters;
    const std::string _mangledName;
    const AASTScope * _body;
    
public:
    
    AASTFunction(const std::string & name,
                 const std::string & type,
                 const std::vector<AASTDeclaration> & parameters,
                 const AASTScope * body);
    
    std::string value(int baseIndent = 0) const;
    std::string declaration() const;
    
};

/* Class definitions */
class AASTClass : public AASTNode {
    
    const std::vector<AASTDeclaration> _attributes;
    const std::string _name;
    const std::string vtable;
    
public:
    
    AASTClass(const std::string & name, const std::vector<AASTDeclaration> & attributes);
    
    std::string value(int baseIndent = 0) const;
};

/* Function calls */
class AASTFuncall : public AASTNode {
    
    const std::vector<AASTNode *> _parameters;
    const std::string _mangledName;
    
public:
    
    AASTFuncall(const std::string & name, const std::string & type, const std::vector<AASTNode *> parameters);
    ~AASTFuncall();
    
    std::string value(int baseIndent = 0) const;
    
};

/* Literals, variables or type names */
class AASTValue : public AASTNode {
    
    const std::string _value;
    
public:
    
    AASTValue(const std::string & value, const std::string & type);
    std::string value(int baseIndent = 0) const;
    
};

/* Variable declarations */
class AASTDeclaration : public AASTNode {
    
    const std::string _name;
    const AASTNode * _value;
    
public:
    
    AASTDeclaration(const std::string & name, const std::string & type, const AASTNode * value);
    ~AASTDeclaration();
    std::string value(int baseIndent = 0) const;
    
};

/* Operator calls */
class AASTOperator : public AASTNode {
  
    const std::vector<AASTNode *> _parameters;
    const std::string _operator;
    
public:
    
    AASTOperator(const std::string & op, const std::string & type, const std::vector<AASTNode *> parameters);
    ~AASTOperator();
    
    std::string value(int baseIndent = 0) const;
    const std::string & getOperator() const;
    
};

/* Type casts */
class AASTCast : public AASTNode {
    
    const AASTNode * _value;
    const std::string _desiredType;
    
public:
    
    AASTCast(const AASTNode * value, const std::string desiredType);
    ~AASTCast();
    
    std::string value(int baseIndent = 0) const;
    
};

#endif /* aast_node_hpp */
