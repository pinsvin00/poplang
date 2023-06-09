//
// Created by pnsv0 on 21/01/2023.
//
#include "Engine.h"


#define CHECK_HOMO_VTYPES(vt,l,r) l->value_type == r->value_type && l->value_type == vt

void SEQL::Engine::raise_error()
{
    if(error.is_critical)
    {
        std::cout << "CRITICAL : ";
    }

    std::cout << this->error.message << std::endl;
    if(this->error.is_critical)
    {
        exit(0);
    }
    
}

SEQL::Value* SEQL::Engine::handle_operator(SEQL::OperatorFragment* frag) {

    if (frag->operator_type == OperatorType::DOT) {
        auto l_val = eval(frag->l_arg);
        //access method from some object
        if(frag->r_arg->type == FragmentType::FUNCTION_CALL) 
        {
            auto function_call = (FunctionCallFragment*)(frag->r_arg);
            auto f_name = function_call->function_name;
            //auto args = resolve_args(function_call->args);
            if(l_val->value_type == ValueType::ARRAY) 
            {
                auto arr = *l_val;
                if(f_name == "pop") 
                {
                    arr.array_values->pop_back();
                    return nullptr;
                }
                else if(f_name == "append") {

                    std::vector<Value*> args;
                    for(auto & element : function_call->args->composed_statements) {
                        Value * val = NEW_VALUE((Value*)eval(element->ast_root));
                        arr.array_values->push_back(val);
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
            if(l_val->value_type == ValueType::STRING) 
            {
                if(r_val->name == "size") 
                {
                    return NEW_VALUE((int32_t)l_val->result_sz);
                }
            }
            else if (l_val->value_type == ValueType::ARRAY)
            {
                auto array_value = *l_val;
                if(r_val->name == "size") 
                {
                    return NEW_VALUE((int32_t)array_value.array_values->size());
                }
            }
        }

    }

    auto l = this->eval(frag->l_arg);
    auto r = this->eval(frag->r_arg);
    Value * result = nullptr;

    if (frag->operator_type == OperatorType::ASSIGN) {
        auto val = Value((Value*)r);
        val.dispose = false;
        *l = val;
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
        result = nullptr;
    }
    else if (frag->operator_type == OperatorType::DECREMENT) {

        if(l->value_type == ValueType::NUMBER)
        {
            memcpy(l->result, int_to_bytes(bytes_to_int(l->result) - 1), sizeof(int));
        }
        else
        {
            sprintf(error.message, "Cannot decrement non number value!");
            error.is_critical = true;
            raise_error();
        }
        result = nullptr;
    }
    //behavior is different among the types
    else if(frag->operator_type == OperatorType::PLUS_EQUAL)
    {
        if(l->value_type == ValueType::NUMBER && l->value_type == r->value_type)
        {
            int32_t r_int = bytes_to_int(r->result);
            int32_t l_int = bytes_to_int(l->result);
            memcpy(l->result, int_to_bytes(l_int + r_int), sizeof(int));
        }
        else if(l->value_type == ValueType::STRING && l->value_type == r->value_type)
        {
            std::string result = std::string(l->result) + std::string(r->result);
            const char* res_cstr = result.c_str();

            //I should change it to realloc, 
            free(l->result);
            l->result = (char*)malloc(strlen(res_cstr) + 1);
            l->result_sz = strlen(res_cstr) + 1;

            memcpy(l->result, res_cstr, l->result_sz);
            printf("%s\n", l->result);
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
        if(r->dispose)
        {
            //delete r;
        }

        return nullptr;
    }
    else if(frag->operator_type == OperatorType::MINUS_EQUAL)
    {
        if(CHECK_HOMO_VTYPES(ValueType::NUMBER, l,r))
        {
            int32_t r_int = bytes_to_int(r->result);
            int32_t l_int = bytes_to_int(l->result);
            memcpy(l->result, int_to_bytes(l_int - r_int), sizeof(int));
        }
        else
        {
            sprintf(error.message, "Cannot use MINUS_EQUAL operator on non number value!");
            error.is_critical = true;
            raise_error();
        }
        return nullptr;
    }
    //(INT, INT) (STRING, STRING)
    if( frag->operator_type == OperatorType::GREATER ||
        frag->operator_type == OperatorType::LESS 
    ) {

        //the base is greater
        if(l->value_type == ValueType::NUMBER && r->value_type == l->value_type)
        {
            auto l_val = bytes_to_int(l->result);
            auto r_val = bytes_to_int(r->result);
            if(frag->operator_type == OperatorType::LESS)
            {
                result = NEW_VALUE(l_val > r_val);
            }
            else if(frag->operator_type == OperatorType::GREATER)
            {
                result = NEW_VALUE(l_val < r_val);
            }

        }
        else if(l->value_type == ValueType::STRING && l->value_type == r->value_type)
        {
            auto l_val = std::string(l->result);
            auto r_val = std::string(r->result);

            if(frag->operator_type == OperatorType::LESS)
            {
                result = NEW_VALUE( strcmp(l->result, r->result) < 0 );
            }
            else if(frag->operator_type == OperatorType::GREATER)
            {
                result = NEW_VALUE( strcmp(l->result, r->result) > 0 );
            }

        }
        else
        {
            //raise error
            this->error.is_critical = true;
            sprintf(this->error.message, "Failed to perform GREATER operator, cannot compare (%s) and (%s)",
                l->value_type, r->value_type
            );
            raise_error();
        }
    }
    //(STRING, STRING) (INT, INT)
    else if (frag->operator_type == OperatorType::ADD) {
        if(CHECK_HOMO_VTYPES(ValueType::NUMBER, l,r))
        {
            int32_t l_val = bytes_to_int(l->result);
            int32_t r_val = bytes_to_int(r->result);
            result = NEW_VALUE(l_val + r_val);
        }
        else if(l->value_type == ValueType::STRING && l->value_type == r->value_type)
        {
            result = NEW_VALUE(std::string(l->result) + std::string(r->result));
        }
        else
        {
            sprintf(error.message, "Invalid usage of add");
            error.is_critical = true;
            raise_error();
        }
    }
    //(INT, INT)
    else if(frag->operator_type == OperatorType::SUB) {
        if(l->value_type == ValueType::NUMBER && l->value_type == r->value_type)
        {
            int32_t l_val = bytes_to_int(l->result);
            int32_t r_val = bytes_to_int(r->result);
            result = NEW_VALUE(l_val - r_val);
        }
        else
        {
            sprintf(error.message, "Invalid usage of \"-\" operator");
            error.is_critical = true;
            raise_error();
        }
        
    }
    //(INT, INT)
    else if(frag->operator_type == OperatorType::MODULO) {
        if(CHECK_HOMO_VTYPES(ValueType::NUMBER, l, r))
        {
            result = NEW_VALUE(bytes_to_int(l->result) % bytes_to_int(r->result));
        }
        else
        {
            sprintf(error.message, "Invalid usage of \"%\" operator, expected (INT, INT) got (%s, %s)", l->value_type, r->value_type);
            error.is_critical = true;
            raise_error();
        }

    }
    //(INT, INT), (STRING, INT)
    else if(frag->operator_type == OperatorType::MUL) {
        if(CHECK_HOMO_VTYPES(ValueType::NUMBER, l,r))
        {
            int32_t l_val = bytes_to_int(l->result);
            int32_t r_val = bytes_to_int(r->result);
            result = NEW_VALUE( l_val * r_val);
        }
        else if(l->value_type == ValueType::STRING && r->value_type == ValueType::NUMBER)
        {
            std::string base = l->result;
            std::string res = "";
            for (size_t i = 0; i < bytes_to_int(r->result); i++)
            {
                res += base;
            }
            result = NEW_VALUE(res);
        }
        else
        {
            sprintf(error.message, "Invalid usage of multiply operator");
            error.is_critical = true;
            raise_error();
        }
    }
    //(ANY, ANY)
    else if(frag->operator_type == OperatorType::EQ) {
        result = NEW_VALUE (strcmp(l->result, r->result) == 0);
    }

    //delete subproducts
    // if(l->dispose) 
    // {
    //     delete l;
    // }
    // if(r->dispose) 
    // {
    //     delete r;
    // }
    // delete l;
    // delete r;

    // l = nullptr;
    // r = nullptr;

    return result;
}

std::vector<SEQL::Value*> SEQL::Engine::resolve_args(SEQL::Statement *statement) {
    std::vector<Value*> values;
    for(auto & element : statement->composed_statements) {
        auto val = eval(element->ast_root);
        values.push_back(val);
    }
    return values;
}

SEQL::Engine::Engine() {
    this->lexer = new Lexer();
    this->ast_creator = new ASTCreator();
}


