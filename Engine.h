//
// Created by pnsv0 on 04.12.2022.
//

#ifndef POPDB_ENGINE_H
#define POPDB_ENGINE_H

#include "expression_common.hpp"
#include "Lexer.hpp"
#include "Asago/parser.h"
#include "Asago/ValuePrinter.h"
#include "cpp_channel.hpp"
#include "gc.h"
#include "utils.hpp"
#include "value.hpp"
#include <string>
#include <map>
#include <algorithm>


extern int   poplex();
extern char* poptext;
extern int   popleng;
extern FILE * popin;
extern std::vector<SEQL::Token> __toks;

namespace SEQL {

    #define NEW_VALUE(args...) ({    \
        Value* val = new Value(args);\
        gc->reg_val(val);            \
        val;                         \
    }                                \
    )

    class RuntimeSEQLError
    {
    public:
        char message[512];
        bool is_critical = false;
    };


    class Scope 
    {
    public:
        std::map<std::string, Variable*> * all_variables;
        std::map<std::string, Variable*> local_variables;

        void drop_local_variables();
    };

    class Engine {
        GarbageCollector* gc = new GarbageCollector();

        ASTCreator * ast_creator = nullptr;
        Value* stored_value = nullptr;
        Asago::Parser parser;
        Asago::ValuePrinter prntr;

        std::vector<Scope*> scopes;
        std::vector<CPPChannel *> channels;

        bool fatal_error_occured = false;
        RuntimeSEQLError error;
        void raise_error();
        void drop_last_scope();
        void make_new_scope();
        std::vector<Value*> resolve_args(Statement * statement);
        void load_default_functions();
        SEQL::Value * start_function(FunctionCallFragment* frag);
    public:
        Engine();
        void execute_file(const std::string& path);
        void execute_statement(Statement * statement);

        void run_function(std::string fun_name, std::vector<SEQL::Value*> vals);


        bool break_requested = false;
        bool continue_requested = false;
        bool return_requested = false;

        Value* handle_operator(OperatorFragment* frag);
        Value* handle_keyword(KeywordFragment* frag);
        Value* eval(Fragment * fragment);

        bool evals_to_true(Fragment* condition_fragment);

        std::map<std::string, Variable*> variables;
        std::map<std::string, Function*> functions;

        Value * str(std::vector<SEQL::Value *> val);
        Value * str(SEQL::Value * val);
        Value * to_int(std::vector<SEQL::Value *> val);
        Value * type_of(std::vector<SEQL::Value *> val);
        Value * format(std::vector<SEQL::Value*> args);
        Value * println(std::vector<SEQL::Value *> val);
        Value * print(std::vector<SEQL::Value *> val);
        Value * obj(std::vector<SEQL::Value*> val);
        Value * make_request(std::vector<SEQL::Value*> val);
        Value * channel_to_cpp(std::vector<SEQL::Value*> val);


        void register_channel(CPPChannel * channel);


        std::string stringifyValue(Value* val);

    };
}



#endif //POPDB_ENGINE_H
