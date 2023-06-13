#include "Engine.h"
#include "expression_common.hpp"
#include <cassert>
#define VAL_TO_CSTR(v) (                            \
    {                                               \
        Value* r = eval(v);                         \
        assert(r->value_type == ValueType::STRING); \
        std::string rs = r->result;                 \
        rs;                                         \
    })

#define VAL_TO_INT32_T(v) (                         \
    {                                               \
        Value* r = eval(v);                         \
        assert(r->value_type == ValueType::NUMBER); \
        int32_t rs = bytes_to_int(r->result);       \
        rs;                                         \
    })

SEQL::Value * SEQL::Engine::str(SEQL::Value * value)
{
    if(value->value_type == ValueType::NUMBER)
    {
        int32_t b = bytes_to_int(value->result);
        return NEW_VALUE(std::to_string(b));
    }
    else if(value->value_type == ValueType::STRING)
    {
        return value;
    }
    //This is very dangerous line, will cause infinite recursion
    else if(value->value_type == ValueType::ARRAY)
    {
        std::string buffer = "[ ";
        auto deref = value->array_values;
        for (size_t i = 0; i < deref->size(); i++)
        {
            buffer += std::string(str(value->array_values->at(i))->result) + ", ";
        }
        buffer.pop_back();
        buffer.pop_back();
        buffer += " ]";

        return NEW_VALUE(buffer);
    }
    else if(value->value_type == ValueType::OBJ)
    {
        std::string buffer = "{";
        auto & deref = *value->mapped_values;
        for (auto pair : deref)
        {
            buffer +=  "\"" + pair.first + "\"" + " : " + stringifyValue(pair.second) + " , ";
        }
        buffer.pop_back();
        buffer.pop_back();
        buffer += "}";

        return NEW_VALUE(buffer);
    }
    else if(value->value_type == ValueType::BOOL)
    {
        char st = value->result[0];
        std::string value = st == 1 ? "true" : "false";
        return NEW_VALUE(value);
    }

}

SEQL::Value * SEQL::Engine::str(std::vector<SEQL::Value *> val) 
{
    if(val.size() != 1) {
        sprintf(error.message, "Too many arguments for str() functions, expected one.");
        error.is_critical = true;
        raise_error();
    }
    return str(val[0]);
}

SEQL::Value * SEQL::Engine::to_int(std::vector<SEQL::Value *> val) 
{
    if(val.size() != 1) {
        raise_error();
    }
    auto arg1 = val[0];
    auto value = eval(arg1);

    if(value->value_type == ValueType::NUMBER)
    {
        //delete value;
        return new Value(value);
    }
    else if(value->value_type == ValueType::STRING)
    {
        try {
            auto res = std::stoi(value->result);
            //delete value;
            return new Value(res);
        }
        catch(std::exception & ex)
        {
            sprintf(this->error.message, "Failed to convert string value %s to int", value->result); 
            this->error.is_critical = true;
            raise_error();
        }

    }
    else if(value->value_type == ValueType::BOOL)
    {
        return new Value(bytes_to_int(value->result));
    }
    else
    {
        sprintf(this->error.message, "Cannot convert value with %s type to int");
        this->error.is_critical = true;
        raise_error();
    }

    delete value;
}
SEQL::Value * SEQL::Engine::type_of(std::vector<SEQL::Value *> val)
{
    if(val.size() != 1) {
        raise_error();
    }
    auto arg1 = val[0];
    if(arg1->value_type == ValueType::NUMBER)
    {
        return new Value("number");
    }
    else if(arg1->value_type == ValueType::STRING)
    {
        return new Value("string");
    }
    else if(arg1->value_type == ValueType::BOOL)
    {
        return new Value("boolean");
    }
    else if(arg1->value_type == ValueType::ARRAY)
    {
        return new Value("array");
    }
    else if(arg1->value_type == ValueType::UNSPECIFIED) 
    {
        return new Value("unspecified");
    }
}
SEQL::Value * SEQL::Engine::format( std::vector<SEQL::Value*> args)
{
    std::string format = stringifyValue(args[0]);
    size_t pos = 0;
    size_t ith_arg = 1;
    std::string delim = "$_";
    std::string result;

    std::string chunk;
    auto start = 0U;
    auto end = format.find(delim);
    while (end != std::string::npos)
    {
        if(args.size() <= ith_arg) {
            sprintf(error.message, "Too few arguments for \"format\" function");
            error.is_critical = true;
            raise_error();
        }
        result += format.substr(start, end - start) + stringifyValue(args[ith_arg]);
        start = end + delim.length();
        ith_arg ++;
        end = format.find(delim, start);
    }

    result += format.substr(start, end);
    return new Value(result);
}

SEQL::Value * SEQL::Engine::println(std::vector<SEQL::Value *> val)
{
    if(val.size() != 1) {
        raise_error();
    }
    auto r = str(val)->result;
    std::cout << r << std::endl;
}
SEQL::Value * SEQL::Engine::print(std::vector<SEQL::Value *> val)
{
    if(val.size() != 1) {
        raise_error();
    }
    std::cout << str(val)->result;
}

SEQL::Value * SEQL::Engine::obj(std::vector<SEQL::Value*> val)
{
    //todo copy object from arguments
    Value * value = new Value();
    value->value_type = ValueType::OBJ;
    value->mapped_values = new std::map<std::string, Value*>();
    return value;
}


// SEQL::Value * SEQL::Engine::set_seed(SEQL::Value * seed)
// {

// }
// SEQL::Value * SEQL::Engine::random(SEQL::Value * seed)
// {

// }
// SEQL::Value * SEQL::Engine::exit(SEQL::Value * exit_code)
// {
//     exit(0);
// }
// SEQL::Value * SEQL::Engine::scanf(SEQL::Value * templ, std::vector<SEQL::Value*> args)
// {

//     return nullptr;
// }
