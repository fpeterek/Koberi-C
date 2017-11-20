//
//  AnalyzedAbstractSyntaxTree.cpp
//  koberi-c
//
//  Created by Filip Peterek on 30/10/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#include "analyzed_abstract_syntax_tree.hpp"

AnalyzedAbstractSyntaxTree::~AnalyzedAbstractSyntaxTree() {
    
    for (AASTClass * c : _classes) {
        delete c;
    }
    
    for (AASTDeclaration * d : _globalDeclarations) {
        delete d;
    }
    
    for (AASTFunction * f : _functions) {
        delete f;
    }
    
}

void AnalyzedAbstractSyntaxTree::setImportedLibs(std::vector<std::string> & libs) {
    _cLibs = &libs;
}

const std::vector<std::string> & AnalyzedAbstractSyntaxTree::getImportedLibs() {
    return *_cLibs;
}

void AnalyzedAbstractSyntaxTree::emplaceClass(AASTClass * c) {
    _classes.emplace_back(c);
}

void AnalyzedAbstractSyntaxTree::emplaceGlobalDeclaration(AASTDeclaration * declaration) {
    _globalDeclarations.emplace_back(declaration);
}

void AnalyzedAbstractSyntaxTree::emplaceFunction(AASTFunction * function) {
    _functions.emplace_back(function);
}

const std::vector<AASTClass *> & AnalyzedAbstractSyntaxTree::getClasses() {
    return _classes;
}

const std::vector<AASTDeclaration *> & AnalyzedAbstractSyntaxTree::getDeclarations() {
    return _globalDeclarations;
}

const std::vector<AASTFunction *> & AnalyzedAbstractSyntaxTree::getFunctions() {
    return _functions;
}

