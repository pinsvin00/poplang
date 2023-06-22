#include "value.hpp"
using namespace SEQL;

Value::~Value()
{
    if (dispose == true)
    {
        delete result;
        delete array_values;
        delete mapped_values;
    }
};
Value::Value(bool tf)
{
    this->value_type = ValueType::BOOL;
    this->result = new char[1];
    this->result_sz = 1;
    this->result[0] = (int)tf;
}
Value::Value(std::string value)
{
    this->value_type = ValueType::STRING;
    this->result = new char[value.size() + 1];
    this->result_sz = value.size() + 1; // include "\0"
    memcpy(this->result, value.c_str(), value.size() + 1);
}
Value::Value(double value)
{
    this->value_type = ValueType::DOUBLE;
    this->result = new char[sizeof(value)];
    this->result_sz = sizeof(value);
    memcpy(this->result, &value, sizeof(value));
}
Value::Value(int32_t value)
{
    this->value_type = ValueType::NUMBER;
    this->result = new char[sizeof(value)];
    this->result_sz = sizeof(value);
    memcpy(this->result, &value, sizeof(value));
}
Value::Value(char *arr, size_t sz, ValueType val_type, bool copy)
{
    this->value_type = val_type;
    this->result_sz = sz;
    if (copy)
    {
        this->result = new char[sz];
        memcpy(arr, this->result, sz);
    }
    else
    {
        this->result = arr;
    }
    this->result = arr;
}
Value::Value(Value *val, bool copy)
{
    this->value_type = val->value_type;
    this->result_sz = val->result_sz;
    if (val->value_type == ValueType::ARRAY)
    {
        if (copy)
        {
            this->array_values = new std::vector<Value *>();
            auto &values = *val->array_values;
            for (size_t i = 0; i < values.size(); i++)
            {
                this->array_values->push_back(new Value(values[i], copy));
            }
        }
        else
        {
            this->array_values = val->array_values;
        }
    }
    else if (val->value_type == ValueType::OBJ)
    {
        if (copy)
        {
            // implement this!
            this->mapped_values = val->mapped_values;
        }
        else
        {
            this->mapped_values = val->mapped_values;
        }
    }
    else
    {
        if (copy)
        {
            this->result = new char[result_sz];
            memcpy(this->result, val->result, result_sz);
        }
        else
        {
            this->result = val->result;
        }
        this->array_values = val->array_values;
    }
}