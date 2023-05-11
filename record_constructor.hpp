#ifndef RECORD_CONSTRUCTOR_H
#define RECORD_CONSTRUCTOR_H

#include <iostream>
#include <string>
#include <vector>
#include "base_common.hpp"
#include "utils.hpp"


namespace Base {
    class RecordConstructor {
        public:
        RecordConstructor() = default;
        Record construct(RecordDefinition definition,std::vector<Token> arguments);
        std::vector<char> field_to_bytes(RecordField &field, Token & frag );
    };
}

#endif