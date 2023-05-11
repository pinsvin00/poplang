
#ifndef FUNCTION_REGISTRY_H
#define FUNCTION_REGISTRY_H

#include <map>
#include <string>

namespace Base {
    enum class NativeFunctionIdentifier {
        INSERT_BEGIN,
        INSERT,
        SELECT_NTH,
        POP,
        POP_NTH,
        DELETE_NTH,
    };
}
#endif