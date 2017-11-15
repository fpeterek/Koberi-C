//
//  translator.cpp
//  koberi-c
//
//  Created by Filip Peterek on 22/07/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#include "translator.hpp"

Translator::Translator(TraversableAbstractSyntaxTree & ast,
                       AnalyzedAbstractSyntaxTree & aast) : _ast(ast), _aast(aast) {
    
}

void Translator::libraries() {
    
    _output << "/* Libraries */" << "\n\n";
    
    _output << "#include <stdio.h>"  << "\n";
    _output << "#include <stdlib.h>" << "\n";
    _output << "#include <time.h>"   << "\n";
    _output << "#include <math.h>"   << "\n";
    _output << "#include <string.h>" << "\n";
    _output << "#include <stdint.h>" << "\n";
    
}

void Translator::typedefs() {
    
    _output << "\n" << "/* Typedefs */" << "\n\n";
    
    _output << "typedef double " + syntax::floatType + ";"   << "\n";
    _output << "typedef intmax_t " + syntax::intType + ";"   << "\n";
    _output << "typedef uintmax_t " + syntax::uintType + ";" << "\n";
    _output << "typedef unsigned char uchar;"                << "\n";
    
}

void Translator::translateClasses() {
    
    const std::vector<AASTClass *> & classes = _aast.getClasses();
    
    _output << "\n\n" << "/* Classes */" << "\n\n";
    
    for (AASTClass * cls : classes) {
        _output << cls->value() << std::endl;
    }
    
}

void Translator::translateGlobals() {
    
    const std::vector<AASTDeclaration *> & declarations = _aast.getDeclarations();
    
    _output << "\n\n" << "/* Global Variables */" << "\n\n";
    
    for (AASTDeclaration * declaration : declarations) {
        _output << declaration->value() << ";\n";
    }
    
    std::endl(_output);
    
}

void Translator::translateFunctionDeclarations() {
    
    _output << "\n\n" << "/* Function Declarations */" << "\n\n";
    
    const std::vector<AASTFunction *> & functions = _aast.getFunctions();
    
    for (AASTFunction * function : functions) {
        
        _output << function->declaration() << ";\n";
        
    }
    
    std::endl(_output);
    
}

void Translator::translateFunctions() {
    
    _output << "\n\n" << "/* Function Definitionss */" << "\n\n";
    
    const std::vector<AASTFunction *> & functions = _aast.getFunctions();
    
    for (AASTFunction * function : functions) {
        _output << function->value() << "\n" << std::endl;
    }
    
}

void Translator::main() {
    
    _output << "\n" << "/* C Main Function */" << "\n" << "\n";
    
    _output << "int main(int argc, const char * argv[]) {" << "\n";
    _output << INDENT << "return " << NameMangler::mangleName("main", std::vector<parameter>()) << "();" << "\n";
    _output << "}" << std::endl;
    
}

void Translator::setOutputFile(const std::string & filename) {
    
    _output = std::ofstream(filename);
    
    if (not _output) {
        throw file_not_created(filename);
    }
    
}

void Translator::translate() {
    
    libraries();
    typedefs();
    translateClasses();
    translateGlobals();
    translateFunctionDeclarations();
    translateFunctions();
    
    main();
    
}
