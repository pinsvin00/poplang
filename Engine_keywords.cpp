//
// Created by pnsv0 on 21/01/2023.
//

#include "Engine.h"

SEQL::Value* SEQL::Engine::handle_keyword(SEQL::KeywordFragment* frag) {
    if (frag->keyword_type == KeywordType::VAR) {
        auto value = this->eval(frag->arguments[0]);
        auto variable_name = std::string(value->result);
        auto var = new Variable();
        var->value = new Value(0);
        var->name = variable_name;
        this->variables[variable_name] = var;
        return var->value;
    }
    else if(frag->keyword_type == KeywordType::CONTINUE) {
        this->continue_requested = true;
        return nullptr;
    }
    else if(frag->keyword_type == KeywordType::RETURN)
    {
        this->stored_value = this->eval(frag->arguments[0]);
        this->return_requested = true;

        //return prodcues nothing
        return nullptr;
    }
    else if(frag->keyword_type == KeywordType::BREAK) {
        this->break_requested = true;
        return nullptr;
    }
    else if(frag->keyword_type == KeywordType::INPUT) {
        std::string console_input;
        std::cin >> console_input;
        return new Value(console_input);
    }
    else if(frag->keyword_type == KeywordType::PRINT) {
        auto to_print = frag->arguments[0];
        auto value = this->eval(to_print);
        if(value->value_type == ValueType::NUMBER)
        {
            int32_t int_result = bytes_to_int(value->result);
            printf("%d\n", int_result);
        }
        else if(value->value_type == ValueType::STRING)
        {
            printf("%s\n", value->result);
        }
    }


    return nullptr;
}