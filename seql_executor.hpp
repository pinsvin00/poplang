//
// Created by pnsv0 on 15.04.2022.
//

#ifndef POPDB_SEQL_EXECUTOR_HPP
#define POPDB_SEQL_EXECUTOR_HPP

#include "expression_common.hpp"
#include "expression.hpp"
#include "program_flow.hpp"

namespace SEQL {
    class Executor {
    public:
        //std::shared_ptr<Engine> engine;
        std::shared_ptr<Statement> main;
        std::shared_ptr<Statement> current;

        Executor();

        void run();
    };
}



#endif //POPDB_SEQL_EXECUTOR_HPP
