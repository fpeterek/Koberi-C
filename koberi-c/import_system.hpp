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
#include <set>

#include "contains.hpp"
#include "exceptions.hpp"
#include "syntax.hpp"

class ImportSystem {
    
    std::vector<std::string> _importedFiles;
    std::vector<std::string> _externTypes;
    
    bool isImported(const std::string & filename);
    
    void importType(const std::string & type);
    
    std::vector<std::string> importFiles(const std::vector<std::string> & files);
    std::vector<std::string> importTypes(const std::vector<std::string> & types);
    
public:
    
    void parseImports(const std::string & filename);
    
    void appendExtensions();
    
    const std::vector<std::string> & getImportedFiles();
    const std::vector<std::string> & getExternTypes();
    
};

#endif /* import_system_hpp */
