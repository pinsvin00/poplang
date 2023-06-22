#ifndef EXPRESSION_COMMON_H
#define EXPRESSION_COMMON_H

#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <stack>
#include <functional>
#include <memory>
#include <stdio.h>
#include <cstring>
#include "value.hpp"
#include "Lexer.hpp"

namespace SEQL {

    enum class StatementType {
        FOR_STATEMENT,
        IF_STATEMENT,
        ELSE_STATEMENT,
        ELSE_IF_STATEMENT,
        NON_SPECIFIED,
        ARRAY_STATEMENT,
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
        PLUS_EQUAL,
        MINUS_EQUAL,
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

    class Engine;


    class Statement {
    public:
        Statement() = default;
        ~Statement();
        size_t line_no = 0;
        StatementType type = StatementType::NON_SPECIFIED;
        Fragment* ast_root = nullptr;
        Statement * child_statement = nullptr;
        Fragment* condition = nullptr;
        Fragment* on_init = nullptr;
        Fragment* each = nullptr;
        std::vector<Statement * > composed_statements;
        //if "if" statement has else, or else if
        Statement*  continued_statement = nullptr;

    public:
        bool is_composed = false;
    };

    class FunctionCallFragment : public Fragment {
    public:
        ~FunctionCallFragment()
        {
            delete args;
        }
        FunctionCallFragment() {
            this->type = FragmentType::FUNCTION_CALL;
        }
        std::string function_name;
        Statement * args = nullptr;
    };

    struct Function {
        std::string name;
        bool is_native = false;
        Value* (*native_templated_func)(std::vector<Value*>, Engine* self) = nullptr;
        Statement * function_args = nullptr;
        Statement * function_body = nullptr;
    };

    class Variable {
    public:
        std::string name;
        Value* value = nullptr;

        Variable(ValueType type, Value* value_ptr, std::string name);
        Variable() {
            this->value = new Value(0);
        };
    };

    class KeywordFragment : public Fragment {
    public:
        std::vector<Fragment*> arguments;
        KeywordType keyword_type = KeywordType::UNSPECIFIED;

        ~KeywordFragment();
        KeywordFragment() {
            this->type = FragmentType::KEYWORD;
        };

    };

    class VariableReferenceFragment : public Fragment {
    public:
        std::string name;
        explicit VariableReferenceFragment(const std::string &name);
    };

    class DoFragment : public Fragment {
    public:
        Statement * stmt;
        DoFragment(){
            this->type = FragmentType::DO_FRAGMENT;
        }
    };

    class OperatorFragment : public Fragment {
    public:
        Fragment* l_arg = nullptr;
        Fragment* r_arg = nullptr;
        OperatorType operator_type = OperatorType::UNSPECIFIED;
        bool is_one_arg = false;


        ~OperatorFragment();
        OperatorFragment() {
            this->type = FragmentType::OPERATOR;
        }
    };

    class ParenthesesFragment : public Fragment {
        public:
            ~ParenthesesFragment()
            {
                delete inner_frag;
            }
            explicit ParenthesesFragment(Fragment * frag) : inner_frag(frag){
                this->type = FragmentType::PARENTHESES;
            }
            Fragment * inner_frag = nullptr;
            void reset() 
            {
                delete inner_frag;
            }
    };


    class ArrayAccessFragment : public Fragment {
    public:
        Fragment * array_frag = nullptr;
        Statement * index_expr = nullptr;

        ArrayAccessFragment()
        {
            this->type = FragmentType::ARRAY_ACCESS;
        }

    };

    class ArrayFragment : public Fragment {
    public:
        explicit ArrayFragment(Statement * statement) {
            this->statement = statement;
            this->type = FragmentType::ARRAY;
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
        std::string message;
        int line = -1;
        bool is_critical = false;
    };


    struct ASTScope {
        Fragment * last_frag = nullptr;

        Statement * last_statement = nullptr;
        Statement * current_statement = nullptr;

        bool readingFunctionDeclaration = false;
        bool readingArrayRef = false;
    };

    class ASTCreator {

        unsigned int pos = 0;
        unsigned int current_line = 0;
        Fragment * last_frag = nullptr;
        Fragment * semaphore_frag = nullptr;

        Statement * last_statement = nullptr;
        Statement * current_statement = nullptr;

        std::stack<ASTScope*> scopes;

        ASTScope * new_reader_scope();
        ASTScope * pop_reader_scope();
        void load_reader_scope(ASTScope* scope);

        Token next(bool move_iter = true);
        Fragment * next_fragment();
        void read_fragment();
        void rollback();
        void raise_error();

        bool readingFunctionDeclaration = false;
        bool readingArrayRef = false;

    public:
        ASTState state = ASTState::WORK;
        ASTError error;

        std::map<std::string, Function*> declared_functions;
        ASTCreator() = default;
        std::vector<Token> tokens;
        std::vector<Statement *> as_tree;
        void create_ast();
        bool finished_reading_stmt = false;
        Statement * read_statement();

        ~ASTCreator() {
            for(auto & element : as_tree) {
                if(element != nullptr)
                {
                    delete element;
                }
            }
        }
    };
}

#endif