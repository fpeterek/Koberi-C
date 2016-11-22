//
//  main.cpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#include "koberi-c.hpp"

static std::string TEST = "testing_script.kbsc";


int main(int argc, const char * argv[]) {
    
    // std::vector<std::string> numbers = { "123", "4132", "1234423", "4123", "87654", "145", "12", "5" };
    // print(expr::print(numbers));
    
    
    KoberiC ks;
    
    try {
        ks.tokenize(TEST);
        ks.translate();
    } catch (unexpected_token & e) {
        print(e.what());
    } catch (missing_token & e) {
        print(e.what());
    } catch (invalid_operator & e) {
        print(e.what());
    } catch (file_not_opened & e) {
        print(e.what());
    } catch (undeclared_function_call & e) {
        print(e.what());
    } catch (std::runtime_error & e) {
        print(e.what());
    } catch (std::exception & e) {
        print(e.what());
    }
    
#ifdef PRINT_TOKENS
    ks.test();
#endif
    
     // */
     
    return 0;
    
}
