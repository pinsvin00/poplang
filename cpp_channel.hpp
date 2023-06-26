#include "value.hpp"
#include <functional>


class CPPChannel {

    SEQL::Fragment * trigger_check; 

public:
    std::string name;
    std::function<void(std::vector<SEQL::Value*>)> onTrigger;
    void setTriggerCheck(std::string poplang_code);
    CPPChannel(std::string name) : name(name) {};
};
