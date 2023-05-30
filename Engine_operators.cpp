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
            auto args = resolve_args(function_call->args);
            if(l_val->value_type == ValueType::ARRAY) 
            {
                auto arr = *l_val;
                if(f_name == "pop") 
                {
                    arr.array_values->pop_back();
                    return nullptr;
                }
                else if(f_name == "append") {
                    for(auto & element : args) 
                    {
                        arr.array_values->push_back(new Value(element));
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
                    return new Value((int32_t)l_val->result_sz);
                }
            }
            else if (l_val->value_type == ValueType::ARRAY)
            {
                auto array_value = *l_val;
                if(r_val->name == "size") 
                {
                    return new Value((int32_t)array_value.array_values->size());
                }
            }
        }

    }

    auto l = this->eval(frag->l_arg);
    auto r = this->eval(frag->r_arg);
    if (frag->operator_type == OperatorType::ASSIGN) {
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
        return nullptr;
    }
    //behavior is different among the types
    //left mutating operators will be hard as fuck to manage
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
                return new Value(l_val > r_val);
            }
            else if(frag->operator_type == OperatorType::GREATER)
            {
                return new Value(l_val < r_val);
            }

        }
        else if(l->value_type == ValueType::STRING && l->value_type == r->value_type)
        {
            auto l_val = std::string(l->result);
            auto r_val = std::string(r->result);

            if(frag->operator_type == OperatorType::LESS)
            {
                return new Value( strcmp(l->result, r->result) < 0 );
            }
            else if(frag->operator_type == OperatorType::GREATER)
            {
                return new Value( strcmp(l->result, r->result) > 0 );
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
            return  new Value("null");
        }
    }
    //(STRING, STRING) (INT, INT)
    if (frag->operator_type == OperatorType::ADD) {
        if(CHECK_HOMO_VTYPES(ValueType::NUMBER, l,r))
        {
            int32_t l_val = bytes_to_int(l->result);
            int32_t r_val = bytes_to_int(r->result);
            return new Value(l_val + r_val);
        }
        else if(l->value_type == ValueType::STRING && l->value_type == r->value_type)
        {
            return new Value(std::string(l->result) + std::string(r->result));
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
            return new Value(l_val - r_val);
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
            return new Value(bytes_to_int(l->result) % bytes_to_int(r->result));
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
            return new Value(bytes_to_int(l->result) * bytes_to_int(r->result));
        }
        else if(l->value_type == ValueType::STRING && r->value_type == ValueType::NUMBER)
        {
            std::string base = l->result;
            std::string res = "";
            for (size_t i = 0; i < bytes_to_int(r->result); i++)
            {
                res += base;
            }
            return new Value(res);
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
        return new Value (strcmp(l->result, r->result) == 0);
    }

    return new Value("null");
}

std::vector<SEQL::Value*> SEQL::Engine::resolve_args(SEQL::Statement *statement) {
    std::vector<Value*> values;
    for(auto & element : statement->composed_statements) {
        values.push_back(eval(element->ast_root));
    }
    return values;
}

SEQL::Engine::Engine() {
    this->lexer = new Lexer();
    this->ast_creator = new ASTCreator();
}


