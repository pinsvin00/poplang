//
// Created by pnsv0 on 21/01/2023.
//
#include "Engine.h"

void SEQL::Engine::raise_error()
{
    std::cout << error.is_critical ? "CRITICAL : " : " ";
    std::cout << this->error.message << std::endl;
    if(this->error.is_critical)
    {
        exit(0);
    }
    
}

std::shared_ptr<SEQL::Value> SEQL::Engine::handle_operator(SEQL::OperatorFragment* frag) {
    auto l = this->eval(frag->l_arg);
    auto r = this->eval(frag->r_arg);
    if (frag->operator_type == OperatorType::ASSIGN) {
        //ughhhh this will be problematic af!
        *l = *r;
        //assignment produces nothing
        return nullptr; 
    }
    else if (frag->operator_type == OperatorType::INCREMENT) {

        if(l->value_type == ValueType::NUMBER)
        {
            memcpy(l->result, int_to_bytes(bytes_to_int(l->result) + 1), sizeof(int));
        }
        else
        {
            sprintf(error.message, "Cannot increment non number value!");
            error.is_critical = true;
            raise_error();
        }
        return nullptr;
    }
    //behavior is different among the types
    //left mutating operators will be hard as fuck to 
    else if(frag->operator_type == OperatorType::PLUS_EQUAL)
    {
        if(l->value_type == ValueType::NUMBER && l->value_type == r->value_type)
        {
            memcpy(l->result, int_to_bytes(bytes_to_int(l->result) - bytes_to_int(r->result)), sizeof(int));
        }
        else if(l->value_type == ValueType::STRING && l->value_type == r->value_type)
        {
            std::string result = std::string(l->result) + std::string(r->result);
            l->result = (char*)realloc(l->result, result.size());
            l->result_sz = result.size();
        }
        else
        {
            sprintf(
                error.message,
                "Invalid use of PLUS_EQUAL operator, allowed types are (INT, INT) (STRING, STRING) but not (%s, %s)",
                l->value_type, r->value_type
            );

            error.is_critical = true;
            raise_error();
        }
    }
    else if(frag->operator_type == OperatorType::MINUS_EQUAL)
    {
        if(l->value_type == ValueType::NUMBER)
        {
            memcpy(l->result, int_to_bytes(bytes_to_int(l->result) - bytes_to_int(r->result)), sizeof(int));
        }
        else
        {
            sprintf(error.message, "Cannot use MINUS_EQUAL operator on non number value!");
            error.is_critical = true;
            raise_error();
        }
        return nullptr;
    }
    else if (frag->operator_type == OperatorType::ARRAY_REFERENCE) {
        auto l = eval(frag->l_arg);
        if(l == nullptr) {
            this->fatal_error_occured = true;
        }
        auto r = eval(frag->r_arg);
        auto index = bytes_to_int(r->result);
        auto deref = *l->array_values;
        return deref[index];
    }
    else if (frag->operator_type == OperatorType::DOT) {
        auto l_val = eval(frag->l_arg);
        //access method from some object
        if(frag->r_arg->type == FragmentType::FUNCTION_CALL) {
            auto function_call = (FunctionCallFragment*)(frag->r_arg);
            auto f_name = function_call->function_name;
            auto args = resolve_args(function_call->args);
            if(l_val->value_type == ValueType::ARRAY) {
                auto array_value = *l_val;
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
            auto r_val = (VariableReferenceFragment*)(frag->r_arg);
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
    auto l = eval(frag->l_arg);
    auto r = eval(frag->r_arg);

    //(INT, INT) (STRING, STRING)
    if( frag->operator_type == OperatorType::GREATER ||
        frag->operator_type == OperatorType::LESS    ||
        frag->operator_type == OperatorType::EQ
    ) {

        //the base is greater
        if(l->value_type == ValueType::NUMBER && r->value_type == l->value_type)
        {
            auto l_val = bytes_to_int(l->result);
            auto r_val = bytes_to_int(r->result);
            return std::make_shared<Value>(l_val < r_val);
        }
        else if(l->value_type == ValueType::STRING && l->value_type == r->value_type)
        {
            auto l_val = std::string(l->result);
            auto r_val = std::string(r->result);
            return std::make_shared<Value>( strcmp(l->result, r->result) > 0 );
        }
        else
        {
            //raise error
            this->error.is_critical = true;
            sprintf(this->error.message, "Failed to perform GREATER operator, cannot compare (%s) and (%s)",
                l->value_type, r->value_type);
            raise_error();
            return  std::make_shared<Value>("null");
        }
    }
    //(STRING, STRING) (INT, INT)
    if (frag->operator_type == OperatorType::ADD) {
        if(l->value_type == ValueType::NUMBER && l->value_type == r->value_type)
        {
            auto l_val = bytes_to_int(l->result);
            auto r_val = bytes_to_int(r->result);
            return std::make_shared<Value>(l_val + r_val);
        }
        else if(l->value_type == ValueType::STRING && l->value_type == r->value_type)
        {

        }
    }
    //(INT, INT)
    else if(frag->operator_type == OperatorType::SUB) {
        if(l->value_type == ValueType::NUMBER && l->value_type == r->value_type)
        {
            auto l_val = bytes_to_int(l->result);
            auto r_val = bytes_to_int(r->result);
            return std::make_shared<Value>(l_val - r_val);
        }
        
    }
    //(INT, INT)
    else if(frag->operator_type == OperatorType::MODULO) {
        return std::make_shared<Value>(std::to_string (stoi(l->result) % stoi(r->result) ) );
    }
    //(INT, INT), (STRING, INT)
    else if(frag->operator_type == OperatorType::MUL) {
        return std::make_shared<Value>(std::to_string (stoi(l->result) * stoi(r->result) ) );
    }
    //(ANY, ANY)
    else if(frag->operator_type == OperatorType::EQ) {
        return std::make_shared<Value> (strcmp(l->result, r->result) == 0);
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


