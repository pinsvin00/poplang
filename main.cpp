
#include <string>
#include <map>
#include "expression.hpp"
#include "Lexer.hpp"
#include "Engine.h"

using namespace std;

int main(int argc, char** argv) {

    if(argc == 2)
    {
        std::string file_name = argv[1];

        SEQL::Engine engn;
        engn.execute_file(file_name);
    }
    return 0;
}