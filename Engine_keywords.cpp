//
// Created by pnsv0 on 21/01/2023.
//

#include "Engine.h"

std::shared_ptr<SEQL::Value> SEQL::Engine::handle_keyword(std::shared_ptr<SEQL::KeywordFragment> frag) {
    if (frag->keyword_type == KeywordType::VAR) {
        auto value = this->eval(frag->arguments[0]);
        auto variable_name = value->result;
        auto var = std::make_shared<Variable>();
        var->name = variable_name;
        this->variables[variable_name] = var;
        return var->value;
    }
    else if(frag->keyword_type == KeywordType::CONTINUE) {
        this->continue_requested = true;
        return nullptr;
    }
    else if(frag->keyword_type == KeywordType::BREAK) {
        this->break_requested = true;
        return nullptr;
    }
    else if(frag->keyword_type == KeywordType::INPUT) {
        std::string console_input;
        std::cin >> console_input;
        return  std::make_shared<Value>(console_input);
    }
    else if(frag->keyword_type == KeywordType::PRINT) {
        auto to_print = frag->arguments[0];
        auto value = this->eval(to_print);
        std::cout << value->result << std::endl;
    }


    return nullptr;
}