//
// Created by pnsv0 on 21/01/2023.
//
#include "Engine.h"

std::shared_ptr<SEQL::Value> SEQL::Engine::handle_operator(std::shared_ptr<SEQL::OperatorFragment> frag) {
    if (frag->operator_type == OperatorType::ASSIGN) {
        auto l = eval(frag->l_arg);
        auto r_val = eval(frag->r_arg);
        *l = *r_val;
        return nullptr; //assignment produces nothing
    }
    else if (frag->operator_type == OperatorType::INCREMENT) {
        auto l_val = this->eval(frag->l_arg);
        l_val->result = std::to_string(
                std::stoi(l_val->result) + 1
        );
        return nullptr;
    }
    else if (frag->operator_type == OperatorType::ARRAY_REFERENCE) {
        auto l_val = eval(frag->l_arg);
        if(l_val == nullptr) {
            this->fatal_error_occured = true;
        }

        auto r_val = eval(frag->r_arg);

//        if(r_val->value_type != ValueType::NUMBER) {
//            std::cout << "Non integer array indices" << std::endl;
//        }
        auto index = std::stoi(r_val->result);
        return l_val->array_values[index];
    }
    else if (frag->operator_type == OperatorType::DOT) {
        auto l_val = eval(frag->l_arg);
        //access method from some object
        if(frag->r_arg->type == FragmentType::FUNCTION_CALL) {
            auto function_call = std::static_pointer_cast<FunctionCallFragment>(frag->r_arg);
            auto f_name = function_call->function_name;
            auto args = resolve_args(function_call->args);
            if(l_val->value_type == ValueType::ARRAY) {
                auto array_value = l_val;
                if(f_name == "pop") {
                    array_value->array_values.pop_back();
                    return nullptr;
                }
                else if(f_name == "append") {
                    for(auto & element : args) {
                        array_value->array_values.push_back(std::make_shared<Value>(element));
                    }
                    return nullptr;
                }
            }
            else {
                //todo
            }
        }
        //access field from some object
        else {
            auto r_val = std::static_pointer_cast<VariableReferenceFragment>(frag->r_arg);
            if(l_val->value_type == ValueType::STRING) {
                if(r_val->name == "size") {

                }
            }
            else if (l_val->value_type == ValueType::ARRAY) {
                auto array_value = l_val;
                if(r_val->name == "size") {
                    auto value = std::make_shared<Value>();
                    value->result = std::to_string(array_value->array_values.size());
                    value->value_type = ValueType::NUMBER;
                    value->type = FragmentType::VALUE;

                    return value;
                }
            }

        }

    }

    //arithmetic operators...
    auto l_val = eval(frag->l_arg);
    auto r_val = eval(frag->r_arg);

    if(frag->operator_type == OperatorType::GREATER) {
        return  std::make_shared<Value> (stoi(l_val->result) < stoi(r_val->result) ? "true" : "false");
    }
    else if(frag->operator_type == OperatorType::LESS) {
        return std::make_shared<Value>(stoi(l_val->result) > stoi(r_val->result) ? "true" : "false");
    }
    if (frag->operator_type == OperatorType::ADD) {
        return std::make_shared<Value>(std::to_string (stoi(l_val->result) + stoi(r_val->result) ));
    }
    else if(frag->operator_type == OperatorType::SUB) {
        return std::make_shared<Value>(std::to_string (stoi(l_val->result) - stoi(r_val->result) ) );
    }
    else if(frag->operator_type == OperatorType::MODULO) {
        return std::make_shared<Value>(std::to_string (stoi(l_val->result) % stoi(r_val->result) ) );
    }
    else if(frag->operator_type == OperatorType::MUL) {
        return std::make_shared<Value>(std::to_string (stoi(l_val->result) * stoi(r_val->result) ) );
    }
    else if(frag->operator_type == OperatorType::EQ) {
        return std::make_shared<Value> (l_val->result == r_val->result ? "true" : "false");
    }

    return std::make_shared<Value> ("null");
}

std::vector<std::shared_ptr<SEQL::Value>> SEQL::Engine::resolve_args(SEQL::Statement *statement) {
    std::vector<std::shared_ptr<Value>> values;
    for(auto & element : statement->composed_statements) {
        values.push_back(eval(element->ast_root));
    }
    return values;
}

SEQL::Engine::Engine() {
    this->lexer = new Lexer();
    this->ast_creator = new ASTCreator();
}


