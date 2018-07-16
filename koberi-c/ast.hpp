//
//  ast.hpp
//  koberi-c
//
//  Created by Filip Peterek on 02/04/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#ifndef ast_hpp
#define ast_hpp

#include <string>
#include <unordered_map>
#include <vector>

#include "ast_node.hpp"
#include "exceptions.hpp"
#include "class.hpp"
#include "contains.hpp"
#include "name_mangler.hpp"

/* Class AbstractSyntaxTree, which holds parsed code                 */
/* AbstractSyntaxTree is meant to create the tree and hold the data  */
/* Not to work with it                                               */
/* Check out TraverseableAbstractSyntaxTree for a class with methods */
/* which allow you to work with the tree                             */

class AbstractSyntaxTree {
    
protected:
    /* Format: Name - Data Type */
    /* Keeps track of functions with mangled names, whereas globalScope holds names before mangling */
    /* Less memory efficient, but look-up is faster and code looks better (maybe)                   */
    std::unordered_map<std::string, std::string> _functions;
    
    /* Keeps track of all data types, whether native types or user defined types */
    std::vector<std::string> _dataTypes = { "num", "int", "uint", "char", "uchar", "void", "var" };
    
    const std::vector<std::string> _primitiveTypes = { "num", "int", "uint", "char", "uchar", "void" };
    
    /* Checks if said type exists, if not, throws exception */
    /* If a pointer type is passes to this method,          */
    /* it checks if the pointer points to a valid type      */
    void checkType(const std::string & type);
    
    /* Stores classes */
    std::unordered_map<std::string, _class> _classes;
    /* Keeps classes ordered in the order they were defined                                                      */
    /* This is necessary because C doesn't allow declaration of variables of incomplete (forward declared) types */
    /* Another option would be storing the classes in an ordered collection(vector, ast global scope nodes)      */
    std::vector<std::string> _classOrder;
    
    /* Defines the global scope, since the global scope has no parent, parentScope points to 0 */
    ASTScope _globalScope;
    
    /* Pointer to the current scope - scope that's currently being worked with                                    */
    /* Could be an std::reference_wrapper, but that would probably create even more cluttered code than a pointer */
    ASTScope * _currentScope;
    
    
public:
    AbstractSyntaxTree();
    
    /* Imports extern types imported via #extern statement */
    void addExternTypes(const std::vector<std::string> & types);
    
    /* Tree building methods */
    
    void addMethod(const parameter & method, const std::string & className, const std::vector<parameter> & params);
    void addMethod(const std::string & methodType, const std::string & methodName,
                   const std::string & className, const std::vector<parameter> & params);
    
    void addClassAttribute(const parameter & attribute, const std::string & className);
    void addClassAttribute(const std::string & type, const std::string & name, const std::string & className);
    
    void emplaceFunction(const std::string & functionName,
                         const std::string & returnType,
                         const std::vector<parameter> & params,
                         const std::string & className);
    
    void emplaceConstruct(const std::string & construct,
                          ASTNode * condition);
    
    void emplaceClass(const std::string & className,
                      const std::string & superClass);
    
    void emplaceFunCall(const std::string & name,
                        const std::vector<ASTNode *> & params);
    
    void emplaceFunCall(const ASTFunCall & fcall);
    
    void emplaceDeclaration(const std::string & type,
                            const std::string & name,
                            ASTNode * value = nullptr);
    
    void emplaceInitializerCall(const std::string & varName);
    
    /* Methods, which create constructs, which have their own scope, such as if... */
    /* enter the scope of those constructs                                         */
    /* This method reverses the effect and should be called after parsing of such  */
    /* construct is completed                                                      */
    void leaveScope();
    
    /* Emplaces variable into it's scope once the variable is declared */
    /* Used to store name and type of a declared variable              */
    /* This method should be called once the variable is declared      */
    /* so it can't be accessed before declaration                      */
    void emplaceVariableIntoScope(const parameter & var, ASTScope * scope);
    
};

#endif /* ast_hpp */
