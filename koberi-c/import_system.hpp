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

class ImportSystem {
    
    std::vector<std::string> _importedFiles;
    
    bool isImported(const std::string & filename);
    
public:
    
    void parseImports(const std::string & filename);
    
    const std::vector<std::string> & getImportedFiles();
    
};

#endif /* import_system_hpp */
