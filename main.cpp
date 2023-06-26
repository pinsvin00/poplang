
#include <string>
#include <map>
#include "expression.hpp"
#include "Lexer.hpp"
#include "Asago/parser.h"
#include "Asago/ValuePrinter.h"
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

        CPPChannel* channel = new CPPChannel("some_test");
        channel->onTrigger = [](std::vector<SEQL::Value*> vals) {
            auto some_value = bytes_to_int(vals.at(0)->result);
            std::cout << "some_value was channeled "<< some_value << std::endl;
        };

        engn.register_channel(channel);
        engn.execute_file(file_name);
    }
    return 0;
}