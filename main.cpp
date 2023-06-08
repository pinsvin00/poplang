
#include <string>
#include <map>
#include "expression.hpp"
#include "Lexer.hpp"
#include "Engine.h"

using namespace std;

int main(int argc, char** argv) {

    #ifdef DEBUG_LOGGING
    for(int i = 0 ; i < argc ;i ++)
    {
        std::cout << "Argument no. " << i << " " << argv[i] << std::endl;
    }
    #endif

    if(argc >= 2)
    {
        std::string file_name = argv[1];

        SEQL::Engine engn;
        engn.execute_file(file_name);
    }
    return 0;
}