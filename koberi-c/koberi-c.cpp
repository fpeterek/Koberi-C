//
//  koberi-c.cpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#include "koberi-c.hpp"


KoberiC::KoberiC() : _tokenizer(_tokens),
                     _parser(_tokens, _ast),
                     _analyzer(_ast, _aast),
                     _translator(_ast, _aast) {
    
}

void KoberiC::compile(const std::string & filename) {
    
    parseInputFileName(filename);
    
    handleImports(filename);
    tokenize(filename);
    parse();
    analyze();
    translate();
    
}

void KoberiC::handleImports(const std::string & file) {
    
    std::string filename = file.substr(0, file.rfind(".koberice"));
    
    _importSystem.parseImports(filename);
    _importSystem.appendExtensions();
    
    std::vector<std::string> types = _importSystem.getExternTypes();
    std::vector<std::string> libs = _importSystem.getCLibs();

    _ast.addExternTypes(_importSystem.getExternTypes());
    _aast.setImportedLibs(libs);
}

void KoberiC::tokenize(const std::string & filename) {
    
    std::vector<std::string> files = _importSystem.getImportedFiles();
    
    for (long i = files.size() - 1; i >= 0; --i) {
        
        _tokenizer.tokenizeFile(files[i]);
        
        if (expr::isVerbose()) {
            std::cout << "Tokenizing file " << files[i] << std::endl;
        }
        
    }
    
}

void KoberiC::parse() {
    
    if (expr::isVerbose()) {
        std::cout << "\n" << "Parsing..." << std::endl;
    }
    
    _parser.parse();
    
    /* After the AST is created, there is no need to store the tokens any longer */
    /* Deleting tokens to save memory.                                           */
    _tokens = std::vector<token>();
    
}

void KoberiC::analyze() {
    
    if (expr::isVerbose()) {
        std::cout << "\n" << "Analyzing..." << std::endl;
    }
    
    _analyzer.analyze();
    
}

void KoberiC::translate() {
    
    if (expr::isVerbose()) {
        std::cout << "\n" << "Translating..." << std::endl;
    }
    
    
    _translator.translate();
    
}

void KoberiC::parseInputFileName(const std::string & filename) {
    
    size_t extensionIndex = filename.rfind(".");
    
    _filename = filename.substr(0, extensionIndex) + ".c";
    
    if (expr::isVerbose()) {
        std::cout << "\n" << "Handling imports..." << "\n";
    }
    
    _translator.setOutputFile(_filename);
    
}


void KoberiC::test() {
    
    int c = 0;
    
    for (auto & i : _tokens) {
        
        std::cout << "Token { " << i.value + " }" << std::endl;
        
        if (i.value == "(") ++c;
        if (i.value == ")") --c;
        
        if (not c) { std::endl(std::cout); }
        
    }
    
}
