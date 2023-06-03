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
        PARENTHESES,
        ARRAY,
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

    class Fragment {
    public:
        FragmentType type = FragmentType::UNDEF;
        std::string debug_value;
    };



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


    class Value : public Fragment {
    public:
        ~Value() {        
            //delete result;
        };
        Value(bool tf) 
        {
            this->type = FragmentType::VALUE;
            this->value_type = ValueType::BOOL;
            this->result = new char[1];
            this->result_sz = 1;
            this->result[0] = (int)tf;
        }
        Value(std::string value) {
            this->type = FragmentType::VALUE;
            this->value_type = ValueType::STRING;
            this->result = new char[value.size() + 1];
            this->result_sz = value.size() + 1; //include "\0"
            memcpy(this->result, value.c_str(), value.size() + 1);
        }
        Value(int32_t value){
            this->type = FragmentType::VALUE;
            this->value_type = ValueType::NUMBER;
            this->result = new char[sizeof(value)];
            this->result_sz = sizeof(value);
            memcpy(this->result, &value, sizeof(value));

        }
        Value(char* arr, size_t sz, ValueType val_type, bool copy=true)
        {
            this->type = FragmentType::VALUE;
            this->value_type = val_type;
            this->result_sz = sz;
            if(copy)
            {
                this->result = new char[sz];
                memcpy(arr, this->result, sz);
            }
            else
            {
                this->result = arr;
            }
            this->result = arr;
        }
        Value(Value* val, bool copy=true)
        {
            this->type = FragmentType::VALUE;
            this->value_type = val->value_type;
            this->result_sz = val->result_sz;
            if(val->value_type == ValueType::ARRAY)
            {
                if(copy)
                {
                    this->array_values = new std::vector<Value*>();
                    auto& values = *val->array_values;
                    for(size_t i = 0; i < values.size() - 1; i++)
                    {
                        this->array_values->push_back(new Value(values[i], copy));
                    }
                }
                else
                {
                    this->array_values = val->array_values;
                }
            }
            else
            {
                if(copy)
                {
                    this->result = new char[result_sz];
                    memcpy(this->result, val->result , result_sz);
                }
                else
                {
                    this->result = val->result;
                }
                this->array_values = val->array_values;
            }

        }

        Value() = default;
        ValueType value_type = ValueType::UNSPECIFIED;

        //If value is not stored in some variable or is in coded, it should persist
        bool delete_after_op = false; 
        //
        bool is_shared = false;


        size_t result_sz = 0;
        char* result = nullptr;

        //array value
        Statement * array_statement = nullptr;
        std::vector<Value*> * array_values;

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
    public:
        explicit VariableReferenceFragment(const std::string &name);
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

    class ASTCreator {

        unsigned int pos = 0;
        unsigned int current_line = 0;
        Fragment * last_frag = nullptr;
        Fragment * semaphore_frag = nullptr;

        Statement * last_statement = nullptr;
        Statement * current_statement = nullptr;


        Token next(bool move_iter = true);
        Fragment * next_fragment();
        void read_fragment();
        void rollback();
        void raise_error();

        bool readingFunctionDeclaration = false;

    public:
        ASTState state = ASTState::WORK;
        ASTError error;

        std::map<std::string, Function*> declared_functions;
        ASTCreator() = default;
        std::vector<Token> tokens;
        std::vector<Statement *> as_tree;
        void create_ast();
        bool break_statement = false;
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