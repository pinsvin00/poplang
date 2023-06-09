#include "gc.h"


void GarbageCollector::explore_value(SEQL::Value* value)
{
    if(used_values.count(value) == 0) 
    {
        used_values.insert(value);
        if(value->array_values != nullptr)
        {
            auto & deref = *value->array_values;
            for(auto & element : deref){
                explore_value(element);
            }
        }
    }
    else {
        return;
    }
}

void GarbageCollector::run()
{
    used_values.clear();
    for(auto & element : roots) {
        explore_value(element);
    }
    finish();
}

void GarbageCollector::reg_val(SEQL::Value * val)
{
    this->all_values.insert(val);
}

void GarbageCollector::finish()
{
    std::set<SEQL::Value*> unused_values;
    std::set_difference(all_values.begin(), all_values.end(), used_values.begin(), used_values.end(),
        std::inserter(unused_values, unused_values.end())
    );

    for(auto it = unused_values.begin(); it != unused_values.end(); it++)
    {
        auto some = *it;
        if(some->is_mature)
        {
            all_values.erase(*it);
            delete *it;
        }
    }
}

void GarbageCollector::set_roots(std::vector<SEQL::Value*>& roots)
{
    this->roots = roots;
}