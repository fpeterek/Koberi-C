//
//  AnalyzedAbstractSyntaxTree.hpp
//  koberi-c
//
//  Created by Filip Peterek on 30/10/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#ifndef AnalyzedAbstractSyntaxTree_hpp
#define AnalyzedAbstractSyntaxTree_hpp

#include <vector>

#include "aast_node.hpp"

/* AnalyzedAbstractSyntaxTree class is a class which holds already analyzed nodes   */
/* Function names are mangled, types inferred etc.                                  */
/* AnalyzedAST nodes are self-translating                                           */
/* AnalyzedAST nodes can also be used to create an interpreter, as they are already */
/* analyzed */

class AnalyzedAbstractSyntaxTree {
    
    /* Holds classes */
    std::vector<AASTClass *> _classes;
    /* Holds global function declarations */
    std::vector<AASTDeclaration *> _globalDeclarations;
    /* Holds function and member function declarations */
    std::vector<AASTFunction *> _functions;
    
    /* Holds C libraries which should be imported using #include directive */
    std::vector<std::string> * _cLibs;
    
public:
    
    void setImportedLibs(std::vector<std::string> & libs);
    const std::vector<std::string> & getImportedLibs();
    
    /* Methods used to build AnalyzedAST */
    void emplaceClass(AASTClass * c);
    void emplaceGlobalDeclaration(AASTDeclaration * declaration);
    void emplaceFunction(AASTFunction * function);
    
    /* Methods used to access AnalyzedAST nodes */
    const std::vector<AASTClass *> & getClasses();
    const std::vector<AASTDeclaration *> & getDeclarations();
    const std::vector<AASTFunction *> & getFunctions();
    
    ~AnalyzedAbstractSyntaxTree();
    
};

#endif /* AnalyzedAbstractSyntaxTree_hpp */
