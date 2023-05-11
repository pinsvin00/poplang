////
//// Created by pnsv0 on 13.04.2022.
////
//
//#include "program_flow.hpp"
//
//SEQL::IfStatement::IfStatement(std::vector<Token> fragments) {
//    this->condition = fragments;
//}
//
//void SEQL::IfStatement::invoke(std::shared_ptr<Statement> &current, std::shared_ptr<Engine> engine) {
//    debug_log("Working at if " + this->raw);
//    if(engine->evaluates_to_true(this->condition)) {
//        this->lines[0]->invoke(current, engine);
//        debug_log("Evaluated to true");
//    }
//    else {
//        debug_log("Evaluated to false");
//    }
//    debug_log("-----");
//
//    if(this->next != nullptr) {
//        this->next->invoke(current, engine);
//    }
//}
//
//
//void SEQL::Statement::invoke(std::shared_ptr<Statement> &current, std::shared_ptr<Engine> engine) {
//    engine->execute(this->exp->fragments);
//    if(next != nullptr){
//        this->next->invoke(current, engine);
//    }
//
//};
//
//void SEQL::ForStatement::invoke(std::shared_ptr<Statement> &current, std::shared_ptr<Engine> engine) {
//    engine->execute(this->start);
//    while(engine->evaluates_to_true(this->condition)) {
//        this->lines[0]->invoke(current, engine);
//        engine->execute(this->each);
//    }
//
//    if(next != nullptr){
//        this->next->invoke(current, engine);
//    }
//};