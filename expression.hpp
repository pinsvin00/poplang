#ifndef EXPRESSION_H
#define EXPRESSION_H
#include <string>
#include <map>
#include <functional>
#include <algorithm>
#include <vector>
#include <stack>
#include "base_common.hpp"
#include "expression_common.hpp"
#include "base.hpp"
#include "Lexer.hpp"

namespace SEQL {
    enum class CommandIdParserError {
        INVALID_COMMAND_TYPE = 1,
    };





//    class Engine {
//        std::map<std::string, std::shared_ptr<Variable> > variables;
//        std::map<std::string, Keyword>  keywords;
//        std::map<std::string, Function> functions;
//        std::map<std::string, PredefinedFunction> predef_functions;
//        std::map<std::string, OperatorFragment> operators;
//        Lexer lexer;
//    public:
//        Engine();
//        std::map<std::string, std::shared_ptr<Base::Engine>> engine_repository;
//
//        void initialize_keywords();
//        void initialize_operators();
//        void initialize_predef_functions();
//
//        void evaluate_start(const std::shared_ptr<Expression>& start_expr);
//
//        void evaluate(const std::shared_ptr<Expression>& e);
//
//        void determine_variables(const std::shared_ptr<Expression>& expression);
//
//        void create_expression_structure(const std::shared_ptr<Expression>& expr, std::vector<Token> fragments);
//
//        std::shared_ptr<Expression> execute(std::vector<Fragment> fragments);
//        bool evaluates_to_true(std::vector<Token> fragments);
//    };

}




#endif