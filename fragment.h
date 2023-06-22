#include <string>

namespace SEQL{
    enum class FragmentType {
        UNDEF,
        VALUE,
        KEYWORD,
        OPERATOR,
        VARIABLE,
        SQUARE_BRACKET_OPEN,
        SQUARE_BRACKET_CLOSE,
        CURLY_BRACKET_OPEN,
        CURLY_BRACKET_CLOSE,
        BRACKET_OPEN,
        BRACKET_CLOSE,
        PARENTHESES,
        ARRAY,
        FUNCTION_CALL,
        DO_FRAGMENT,
        ARRAY_ACCESS,
    };
    
    class Fragment {
    public:
        FragmentType type = FragmentType::UNDEF;
        std::string debug_value;
    };
}

   
