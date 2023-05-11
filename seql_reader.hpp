//
// Created by pnsv0 on 13.04.2022.
//

#ifndef POPDB_SEQL_READER_HPP
#define POPDB_SEQL_READER_HPP

#include <iostream>
#include <string>
#include <fstream>
#include "expression_common.hpp"
#include "program_flow.hpp"
#include "utils.hpp"
#include "Lexer.hpp"

namespace SEQL {
    class Reader  {

        void process_line(const std::string& line);

        void process_keyword(SEQL::Token fragment, std::vector<SEQL::Token> next_fragments, const std::string& line_raw);

        bool reading_if;
        bool halt;
    public:
        void loadFile(const std::string& file_dir);
        void append_link(const std::shared_ptr<Statement>& statement);
        int line;
        /*
             * Loads file and parses its structure into SEQL_FILE
             *
             */
        Reader();
        std::stack<std::shared_ptr<Statement>> group_stack;
    };


}

#endif //POPDB_SEQL_READER_HPP
