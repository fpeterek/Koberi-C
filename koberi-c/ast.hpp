//
//  ast.hpp
//  koberi-c
//
//  Created by Filip Peterek on 02/04/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#ifndef ast_hpp
#define ast_hpp

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <functional>

#include "ast_node.hpp"
#include "exceptions.hpp"


class AbstractSyntaxTree {
    
protected:
    /* Format: Name - Data Type */
    /* Keeps track of functions with mangled names, whereas globalScope holds names before mangling */
    /* Less memory efficient, but look-up is faster and code looks better (maybe)                   */
    std::unordered_map<std::string, std::string> _functions;
    
    /* Keeps track of all data types, whether native types or user defined types */
    std::vector<std::string> _dataTypes;
    
    /* Defines the global scope, since the global scope has no parent, parentScope points to 0 */
    ASTScope _globalScope;
    
    /* Pointer to the current scope */
    /* I could also use an std::reference_wrapper, but that would probably create even more cluttered code than a pointer */
    ASTScope * _currentScope;
    
    std::string mangleName(const std::string & name, const std::vector<parameter> & parameters);
    
public:
    AbstractSyntaxTree();
    
    void emplaceFunction(const std::string & functionName,
                         const std::string & returnType,
                         const std::vector<parameter> & params);
    
    void emplaceConstruct(const std::string & construct,
                          const ASTFunCall & condition);
    
    void emplaceClass(const std::string & className,
                      const std::vector<parameter> & attributes);
    
    void emplaceFunCall(const std::string & name,
                        const std::vector<ASTNode *> & params);
    
    void emplaceDeclaration(const std::string & type,
                            const std::string & name,
                            const std::string & value = "");
    
    void leaveScope();
    
    
};

#endif /* ast_hpp */
