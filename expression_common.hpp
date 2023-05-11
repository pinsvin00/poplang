#ifndef EXPRESSION_COMMON_H
#define EXPRESSION_COMMON_H

#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <stack>
#include <functional>
#include <memory>
#include "Lexer.hpp"

namespace SEQL {

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

        STATEMENT_LINK,
        FUNCTION_CALL,
    };

    enum class StatementType {
        FOR_STATEMENT,
        IF_STATEMENT,
        ELSE_STATEMENT,
        ELSE_IF_STATEMENT,
        NON_SPECIFIED,
        ARRAY_STATEMENT,
    };


    enum class ValueType {
        NUMBER,
        STRING,
        BOOL,
        ARRAY,
        UNSPECIFIED,
    };

    enum class OperatorType {
        ADD,
        SUB,
        MUL,
        DIV,
        ASSIGN,
        NOT,
        EQ,
        INCREMENT,
        DECREMENT,
        UNSPECIFIED,
        ARRAY_REFERENCE,
        GREATER,
        LESS,
        OR,
        AND,
        NEGATE,
        MODULO,
        DOT,
    };

    enum class KeywordType {
        UNSPECIFIED,
        VAR,
        PRINT,
        INPUT,
        IF,
        FOR,
        FUN,
        ELSE_IF,
        ELSE,
        CONTINUE,
        BREAK,
        RETURN,
    };



    class Fragment {
    public:
        FragmentType type = FragmentType::UNDEF;
        std::string debug_value;
    };



    class Statement {
    public:
        Statement() = default;
        ~Statement();

        StatementType type = StatementType::NON_SPECIFIED;
        std::shared_ptr<Fragment> ast_root = nullptr;
        Statement * child_statement = nullptr;
        std::vector<Statement * > composed_statements;

        std::shared_ptr<Fragment> condition = nullptr;
        std::shared_ptr<Fragment> on_init = nullptr;
        std::shared_ptr<Fragment> each = nullptr;

        //if "if" statement has else, or else if
        Statement*  continued_statement;

    public:
        bool is_composed = false;
    };

    struct Function {
        std::string name;
        Statement * function_args = nullptr;
        Statement * function_body = nullptr;
    };

    class FunctionCallFragment : public Fragment {
    public:
        FunctionCallFragment() {
            this->type = FragmentType::FUNCTION_CALL;
        }
        std::string function_name;
        Statement * args = nullptr;
    };


    class Value : public Fragment {
    public:
        virtual ~Value() = default;
        explicit Value(std::string value) : result(std::move(value)) {
            this->type = FragmentType::VALUE;
        }
        Value(std::shared_ptr<Value> val) {
            this->type = val->type;
            this->result = val->result;
            this->array_values = val->array_values;
        }

        Value() = default;
        ValueType value_type = ValueType::UNSPECIFIED;

        std::string result;

        //array value
        Statement * array_statement = nullptr;
        std::vector<std::shared_ptr<Value>> array_values;

    };

    class Variable {
    public:
        std::string name;
        std::shared_ptr<Value> value = nullptr;

        Variable(ValueType type, std::shared_ptr<Value> value_ptr, std::string name);
        Variable() {
            this->value = std::make_shared<Value>();
        };
    };

    class KeywordFragment : public Fragment {
    public:
        std::vector<std::shared_ptr<Fragment>> arguments;
        KeywordType keyword_type = KeywordType::UNSPECIFIED;

        ~KeywordFragment();
        KeywordFragment() {
            this->type = FragmentType::KEYWORD;
        };

    };

    class VariableReferenceFragment : public Fragment {
    public:
        std::string name;
    public:
        explicit VariableReferenceFragment(const std::string &name);
    };

    class OperatorFragment : public Fragment {
    public:
        std::shared_ptr<Fragment> l_arg = nullptr;
        std::shared_ptr<Fragment> r_arg = nullptr;
        OperatorType operator_type = OperatorType::UNSPECIFIED;
        bool is_one_arg = false;


        ~OperatorFragment();
        OperatorFragment() {
            this->type = FragmentType::OPERATOR;
        }
    };




    class ArrayFragment : public Fragment {

    public:
        explicit ArrayFragment(Statement * statement) {
            this->statement = statement;
            this->type = FragmentType::STATEMENT_LINK;
        }

        Statement * statement = nullptr;
    };

    enum class ASTState {
        BROKEN,
        WORK,
        FINISHED,
    };

    class ASTError {
    public:
        ASTError() = default;
        ASTError(const std::string &message, int line, bool isCritical);
    private:

        std::string message;
        int line = -1;
        bool is_critical = false;
    };

    class ASTCreator {

        unsigned int pos = 0;
        std::shared_ptr<Fragment> last_frag = nullptr;
        std::shared_ptr<Fragment> semaphore_frag = nullptr;

        Statement * last_statement = nullptr;
        Statement * current_statement = nullptr;

        ASTState state;
        ASTError current_error;


        Token next(bool move_iter = true);
        std::shared_ptr<Fragment> next_fragment();
        void read_fragment();
        void early_rollback();
    public:
        std::map<std::string, std::shared_ptr<Function>> declared_functions;
        ASTCreator() = default;
        std::vector<Token> tokens;
        std::vector<Statement *> as_tree;
        void create_ast();
        bool break_constructing = false;
        Statement * read_statement();

        ~ASTCreator() {
            early_rollback();
        }
    };
}

#endif