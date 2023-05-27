//
// Created by pnsv0 on 04.12.2022.
//

#ifndef POPDB_ENGINE_H
#define POPDB_ENGINE_H

#include "expression_common.hpp"
#include "Lexer.hpp"
#include "utils.hpp"
#include <map>
#include <algorithm>

namespace SEQL {

    class RuntimeSEQLError
    {
    public:
        char message[512];
        bool is_critical = false;
    };

    class Engine {
        Lexer * lexer;
        ASTCreator * ast_creator;
        bool fatal_error_occured = false;
        RuntimeSEQLError error;
        void raise_error();
        std::vector<Value*> resolve_args(Statement * statement);

    public:
        Engine();
        void execute_file(const std::string& path);
        void execute_statement(Statement * statement);

        bool break_requested = false;
        bool continue_requested = false;

        Value* handle_operator(OperatorFragment* frag);
        Value* handle_keyword(KeywordFragment* frag);
        Value* eval(Fragment * fragment);

        bool evals_to_true(Fragment* condition_fragment);

        std::map<std::string, Variable*> variables;
        std::map<std::string, Function*> functions;
    };
}



#endif //POPDB_ENGINE_H
