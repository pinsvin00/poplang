#ifndef BASE_CONTROLLER_H
#define BASE_CONTROLLER_H

#include <iostream>
#include <functional>

#include "base_common.hpp"
#include "base.hpp"
#include "expression.hpp"
#include "record_constructor.hpp"


namespace Base {
    class Controller {
        public:
        std::shared_ptr<SEQL::Engine> seql_engine;
        std::shared_ptr<Base::Engine> base_engine;

        Controller() = default;
    };
}
#endif