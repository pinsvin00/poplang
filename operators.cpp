////
//// Created by pnsv0 on 06.04.2022.
////
//
//#include "../include/expression_common.hpp"
//#include "../include/expression.hpp"
//
//
//void SEQL::Engine::initialize_keywords() {
//    this->keywords["VAR"] = Keyword(1, []( const std::vector<Expression>& args, std::map<std::string, std::shared_ptr<Variable> >& vars) {
//        if(args.size() != 1){
//            throw std::invalid_argument("SEQL ERROR : var operator requires 1 argument");
//        }
//        vars[args[0].result.result] = std::make_shared<Variable>();
//        Expression expr = Expression();
//        return Token("NIL");
//    });
//
//
//}
//
//void SEQL::Engine::initialize_operators() {
//    this->operators["+"] = Operator(2, [](const std::vector<Expression> & args , std::map<std::string, std::shared_ptr<Variable> >& vars) {
//        if(args[0].result.type != args[1].result.type) {
//            throw std::invalid_argument("SEQL ERROR : + operator cannot perform addition on 2 different types.");
//        }
//        if(args[0].result.type == SEQL::TokenType::STRING) {
//            return Token(args[0].result.result + args[1].result.result);
//        }
//        else if(args[1].result.type == SEQL::TokenType::NUMBER) {
//            return Token(std::to_string(std::stoi(args[0].result.result) + std::stoi(args[1].result.result) ));
//        }
//
//    });
//
////    this->operators["*"] = Operator(2, []( const std::vector<Expression>& args, std::map<std::string, std::shared_ptr<Variable> >& vars) {
////        if(args[0].result.type == SEQL::TokenType::STRING) {
////            throw std::invalid_argument("SEQL ERROR : * operator cannot perform multiplications on any string type");
////        }
////        else if(args[1].result.type == SEQL::TokenType::NUMBER) {
////            return Token(std::to_string( std::stoi(args[0].result.result) * std::stoi(args[1].result.result) ));
////        }
////
////    });
////    this->operators["-"] = Operator(2, [](std::vector<Token> args, std::map<std::string, std::shared_ptr<Variable> >& varss) {
////        if(args.size() != 2){
////            throw std::invalid_argument("SEQL ERROR : - operator requires 2 arguments");
////        }
////        if(args[0].type != SEQL::TokenType::NUMBER || args[1].type != SEQL::TokenType::NUMBER) {
////            throw std::invalid_argument("SEQL ERROR : - operator cannot perform addition on 2 different types.");
////        }
////
////        return Token(std::to_string( std::stoi(args[1].result) - std::stoi(args[0].result) ));
////    });
//    this->operators["="] = Operator(2, []( const std::vector<Expression>& args, std::map<std::string, std::shared_ptr<Variable> >& vars) {
//
//
//        vars[args[1].result.result]->result = args[0].result.result;
//        vars[args[1].result.result]->type  = static_cast<int>(args[0].result.type);
//
//        return Token(std::string("NIL"));
//    });
//
////    this->operators["=="] = Operator(2, [](std::vector<Token> args, std::map<std::string, std::shared_ptr<Variable> >& vars) {
////        if(args.size() != 2){
////            throw std::invalid_argument("SEQL ERROR : == operator requires 2 arguments");
////        }
////
////
////        for(auto & element : args) {
////            if(element.type == FragmentType::VARIABLE) {
////                std::shared_ptr<Variable> v = vars[element.result];
////                element.type = SEQL::FragmentType::LITERAL;
////                element.type = SEQL::TokenType::NUMBER;
////                element.result = v->result;
////            }
////        }
////
////        if(args[0].result == args[1].result) {
////            return Token(std::string("TRUE"));
////        }
////        else {
////            return Token("FALSE");
////        }
////
////    });
//
//}
//
//SEQL::Engine::Engine() {
//    this->initialize_operators();
//    this->initialize_keywords();
//    this->initialize_predef_functions();
//    this->lexer = Lexer();
//}
//
//bool SEQL::Engine::evaluates_to_true(std::vector<Token> fragments) {
//    std::shared_ptr<Expression> expr = this->execute(fragments);
//    if(expr->result.result == "TRUE") {
//        return true;
//    }
//    return false;
//}
//
//void SEQL::Engine::initialize_predef_functions() {
//    this->predef_functions["PRINT"] =
//            SEQL::PredefinedFunction(2, [](std::vector<Expression> args, std::map<std::string,
//            std::shared_ptr<Variable>>& vars)
//            {
//        Expression expr = args[0];
//        for(const auto & element : expr.sub_expr) {
//            debug_log("----\n USING INNER PRINT");
//            std::cout << element->result.result << std::endl;
//            debug_log("----");
//        }
//
//        return Token("NIL");
//    });
//}
//
