#ifndef GC_H
#define GC_H

#include <set>
#include <algorithm>
#include "expression_common.hpp"
#include "value.hpp"

class GarbageCollector {
    //all value pointers
    std::set<SEQL::Value*> all_values;
    std::set<SEQL::Value*> used_values;
    std::vector<SEQL::Value*> roots;

public:
    //should garbage collecting be multithreaded?
    void set_roots(std::vector<SEQL::Value*>& roots);
    void reg_val(SEQL::Value* val);
    void explore_value(SEQL::Value * value);
    void finish();
    void run();
};


#endif