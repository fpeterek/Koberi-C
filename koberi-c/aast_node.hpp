//
//  aast_node.hpp
//  koberi-c
//
//  Created by Filip Peterek on 30/10/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#ifndef aast_node_hpp
#define aast_node_hpp

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <vector>

#define INDENT "    " /* Use four spaces to indent */

enum class AASTNodeType {
    
    Construct,
    Function,
    Scope,
    Class,
    Funcall,
    Value,
    Declaration,
    Operator
    
};

class AASTConstruct;
class AASTFunction;
class AASTScope;
class AASTClass;
class AASTFuncall;
class AASTValue;
class AASTDeclaration;
class AASTOperator;

class AASTNode {
    
    const AASTNodeType _nodeType;
    const std::string _type;
    
public:
    
    AASTNode(AASTNodeType nodeType, const std::string & dataType);
    virtual ~AASTNode() = 0;
    
    AASTNodeType nodeType() const;
    std::string type() const;
    
    virtual std::string value(int baseIndent) const = 0;
    
};

class AASTScope : public AASTNode {
    
    const std::vector<AASTNode *> _body;
    
public:
    
    AASTScope(const std::vector<AASTNode *> & body);
    ~AASTScope();
    std::string value(int baseIndent) const;
    
};

class AASTConstruct : public AASTNode {
    
    const std::string _construct;
    const AASTNode * _condition;
    const AASTScope _body;
    
public:
    
    AASTConstruct(const std::string & construct, const AASTNode * condition, const AASTScope & body);
    ~AASTConstruct();
    
    std::string value(int baseIndent) const;
    
};

class AASTFunction : public AASTNode {
    
    const std::vector<AASTDeclaration> _parameters;
    const std::string _mangledName;
    const AASTScope _body;
    
public:
    
    AASTFunction(const std::string & name,
                 const std::string & type,
                 const std::vector<AASTDeclaration> & parameters,
                 const AASTScope & body);
    
    std::string value(int baseIndent) const;
    
};

class AASTClass : public AASTNode {
    
    const std::vector<AASTDeclaration> _attributes;
    const std::string _name;
    
public:
    
    AASTClass(const std::string & name, const std::vector<AASTDeclaration> & attributes);
    
    std::string value(int baseIndent) const;
};

class AASTFuncall : public AASTNode {
    
    const std::vector<AASTNode *> _parameters;
    const std::string _mangledName;
    
public:
    
    AASTFuncall(const std::string & name, const std::string & type, const std::vector<AASTNode *> parameters);
    ~AASTFuncall();
    
    std::string value(int baseIndent) const;
    
};

class AASTValue : public AASTNode {
    
    const std::string _value;
    
public:
    
    AASTValue(const std::string & value, const std::string & type);
    std::string value(int baseIndent) const;
    
};

class AASTDeclaration : public AASTNode {
    
    const std::string _name;
    const AASTNode * _value;
    
public:
    
    AASTDeclaration(const std::string & name, const std::string & type, const AASTNode * value);
    ~AASTDeclaration();
    std::string value(int baseIndent) const;
    
};

class AASTOperator : public AASTNode {
  
    const std::vector<AASTNode *> _parameters;
    const std::string _operator;
    
public:
    
    AASTOperator(const std::string & op, const std::string & type, const std::vector<AASTNode *> parameters);
    ~AASTOperator();
    
    std::string value(int baseIndent) const;
    
};

#endif /* aast_node_hpp */