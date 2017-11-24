//
//  import_system.hpp
//  koberi-c
//
//  Created by Filip Peterek on 14/09/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#ifndef import_system_hpp
#define import_system_hpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "contains.hpp"
#include "exceptions.hpp"
#include "syntax.hpp"

/* ImportSystem class handles all imports                               */
/* ImportSystem parses files for import statements,                     */
/* imports Kobeři-C libraries, C libraries and C data types and ensures */
/* no file or type is imported more than once                           */

class ImportSystem {
    
    /* Holds names of imported Kobeři-C file names */
    std::vector<std::string> _importedFiles;
    
    /* Holds names of imported extern data types */
    std::vector<std::string> _externTypes;
    
    /* Holds imported C libraries */
    std::vector<std::string> _cLibs = { "<stdio.h>", "<stdlib.h>", "<time.h>", "<math.h>", "<string.h>", "<stdint.h>" };
    
    /* Checks if a Kobeři-C file has already been imported */
    bool isImported(const std::string & filename);
    
    /* Imports C libraries or C data types unless they have already been imported */
    void importType(const std::string & type);
    void importLib(const std::string & lib);
    
    /* Checks if vector includes value, if not, emplaces value into vector */
    /* Called from importType() and importLib()                            */
    void importInto(std::vector<std::string> & collection, const std::string & value);
    
    /* Iterates over a vector of imported items, picks out data types and returns a vector */
    /* which contains only types                                                           */
    std::vector<std::string> importTypes(const std::vector<std::string> & types);
    
    /* Iterates over a vector of imported items, picks out library imports */
    /* and returns them in a vector                                        */
    std::vector<std::string> importLibraries(const std::vector<std::string> & libs, bool isStdLib = false);
    
public:
    
    void parseImports(const std::string & filename);
    
    /* Appends .koberice extensions to imported files, because in Kobeři-C */
    /* names of imported files are written without the extension           */
    void appendExtensions();
    
    const std::vector<std::string> & getImportedFiles();
    std::vector<std::string> & getExternTypes();
    std::vector<std::string> & getCLibs();
    
};

#endif /* import_system_hpp */
