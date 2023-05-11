//
// Created by pnsv0 on 06.04.2022.
//


#include <utility>
#include "expression_common.hpp"


using namespace SEQL;


/*
 * There are some issues,
 * Everything is just treated like a fragment
 */

void SEQL::ASTCreator::read_fragment() {

    //statement may be multi line lol!
    while (pos < tokens.size() && !break_constructing) {
        Token token = next();
        //primitives
        if(token.type == TokenType::NUMBER) {
            this->last_frag = std::make_shared<Value>(token.value);
            this->last_frag->debug_value = token.value;
        }
        else if(token.type == TokenType::STRING) {
            this->last_frag = std::make_shared<Value>(token.value);
            this->last_frag->debug_value = token.value;
        }
        else if(token.type == TokenType::KEYWORD){
            auto keyword_frag =  std::make_shared<KeywordFragment>();
            if(token.value == "var") {
                Token var_tok = next();
                auto var_name = var_tok.value;
                keyword_frag->keyword_type = KeywordType::VAR;
                keyword_frag->arguments = { std::make_shared<Value>(var_name) };
                keyword_frag->debug_value = "var " + var_name;
            }
            else if(token.value == "input") {
                keyword_frag->keyword_type = KeywordType::INPUT;
                keyword_frag->arguments = {};
                keyword_frag->debug_value = "input! ";
            }
            else if(token.value == "print") {
                keyword_frag->keyword_type = KeywordType::PRINT;
                read_fragment();
                keyword_frag->arguments = { this->last_frag };
                if(last_frag != nullptr) {
                    keyword_frag->debug_value = "print! " + this->last_frag->debug_value;
                }
                else {
                    std::cout << "Too few arguments for print" << std::endl;
                    this->break_constructing = true;
                    return;
                }

            }
            else if(token.value == "if") {
                auto if_statement = this->current_statement;
                if_statement->is_composed = true;
                if_statement->composed_statements = {};

                read_fragment();
                auto condition_fragment = this->last_frag;
                auto composed_statement = this->read_statement();

                //now we have to read the next statement
                if_statement->type = StatementType::IF_STATEMENT;
                if_statement->child_statement = composed_statement;
                if_statement->condition = condition_fragment;

                this->current_statement = if_statement;
                this->last_frag = keyword_frag;
                //no need to continue, since the statement has been fully read
                this->break_constructing = true;
                this->semaphore_frag = nullptr;

                return;

            }
            else if(token.value == "else") {
                //check if next token is if
                auto if_statement = this->last_statement;
                auto else_statement = this->current_statement;
                else_statement->is_composed = true;
                else_statement->condition = nullptr;

                //read semaphore
                read_fragment();
                //last fragment isn't semaphore
                if(last_frag->type == FragmentType::KEYWORD) {
                    read_fragment();
                    auto condition = next_fragment();
                }
                auto composed_statement = this->read_statement();



                //now we have to read the next statement
                else_statement->type = StatementType::ELSE_STATEMENT;
                else_statement->child_statement = composed_statement;

                if_statement->continued_statement = else_statement;

                this->last_frag = keyword_frag;
                //no need to continue, since the statement has been fully read
                this->break_constructing = true;
                return;

            }
            else if(token.value == "break") {
                keyword_frag->keyword_type = KeywordType::BREAK;
                keyword_frag->debug_value = "break";
            }
            else if(token.value == "continue") {
                auto frag = std::make_shared<KeywordFragment>();
                frag->keyword_type = KeywordType::CONTINUE;

                this->last_frag = frag;
            }
            else if(token.value == "return") {
                auto return_frag = std::make_shared<KeywordFragment>();
                read_fragment();
                auto last = this->last_frag;
                return_frag->keyword_type = KeywordType::RETURN;
                return_frag->arguments = {last};

                this->last_frag = return_frag;
            }
            else if(token.value == "fun") {
                auto function = std::make_shared<Function>();
                auto name = std::static_pointer_cast<VariableReferenceFragment>(next_fragment());

                function->name = name->name;
                function->function_args = read_statement();
                read_fragment(); //read {
                //if last_frag not equal to {
                //throw error
                function->function_body = read_statement();

                this->declared_functions[function->name] = function;
                this->last_frag = nullptr;
                return;
            }
            else if(token.value == "for") {
                std::vector<std::shared_ptr<Fragment>> for_fragments;
                for (int i = 0; i < 3; ++i) {
                    read_fragment();
                    for_fragments.push_back(this->last_frag);
                    break_constructing = false;
                }
                auto for_statement = this->current_statement;
                if(for_statement != nullptr && for_fragments.size() == 3) {
                    for_statement->is_composed = true;
                    for_statement->type = StatementType::FOR_STATEMENT;

                    for_statement->on_init = for_fragments[0];
                    for_statement->condition = for_fragments[1];
                    for_statement->each = for_fragments[2];
                    for_statement->is_composed = true;

                    auto statement = this->read_statement();
                    for_statement->child_statement = statement;
                }
                else {
                    //handle errors todo

                    this->state = ASTState::BROKEN;
                    this->current_error = ASTError("Invalid arguments", 0, true);
                    this->break_constructing = true;
                    return;
                }

                this->last_frag = keyword_frag;
                this->break_constructing = true;
                this->semaphore_frag = nullptr;

                return;
            }
            this->last_frag = keyword_frag;
        }
        else if(token.type == TokenType::CURLY_BRACKET) {
            if(token.value == "{") {
                break_constructing = true;
                semaphore_frag = std::make_shared<Fragment>();
                semaphore_frag->type = FragmentType::CURLY_BRACKET_OPEN;
                return;
            }
            else if(token.value == "}") {
                break_constructing = true;
                semaphore_frag = std::make_shared<Fragment>();
                semaphore_frag->type = FragmentType::CURLY_BRACKET_CLOSE;
                return;
            }
        }
        else if (token.type == TokenType::BRACKET) {

            if(token.value == "(") {
                break_constructing = true;
                semaphore_frag = std::make_shared<Fragment>();
                semaphore_frag->type = FragmentType::BRACKET_OPEN;
                return;
            }

            else if(token.value == ")") {
                break_constructing = true;
                semaphore_frag = std::make_shared<Fragment>();
                semaphore_frag->type = FragmentType::BRACKET_CLOSE;
                return;
            }

            if (token.value == "[") {
                //check if we use access operator, or start array
                if(this->last_frag != nullptr) {

                    bool isArrayValue = false;
                    if(this->last_frag->type == FragmentType::OPERATOR) {
                        auto oper = std::static_pointer_cast<OperatorFragment>(this->last_frag);
                        isArrayValue = oper->operator_type == OperatorType::ARRAY_REFERENCE;
                    }

                    if(this->last_frag->type == FragmentType::VARIABLE || this->last_frag->type == FragmentType::STATEMENT_LINK || isArrayValue) {
                        auto array_operator = std::make_shared<OperatorFragment>();
                        array_operator->operator_type  = OperatorType::ARRAY_REFERENCE;
                        array_operator->l_arg = this->last_frag;
                        array_operator->r_arg = this->next_fragment();
                        array_operator->debug_value = array_operator->l_arg->debug_value + array_operator->r_arg->debug_value;

                        this->semaphore_frag = nullptr;

                        this->last_frag = array_operator;
                        continue;
                    }
                }

                semaphore_frag = std::make_shared<Fragment>();
                semaphore_frag->type = FragmentType::SQUARE_BRACKET_OPEN;
                auto array_statement = read_statement();
                auto link = std::make_shared<ArrayFragment>(array_statement);
                this->last_frag = link;

                return;
            }
            else if(token.value == "]" && this->last_frag->type == FragmentType::VARIABLE || this->last_frag->type == FragmentType::VALUE ) {
                return;
            }
            else if (token.value == "]") {
                semaphore_frag = std::make_shared<Fragment>();
                semaphore_frag->type = FragmentType::SQUARE_BRACKET_CLOSE;
                this->break_constructing = true;
                return;
            }
        }
        else if(token.type == TokenType::OPERATOR) {
            auto op = std::make_shared<OperatorFragment>();
            if (token.value == "+") {
                op->operator_type = OperatorType::ADD;
            }
            else if(token.value == "=") {
                op->operator_type = OperatorType::ASSIGN;
            }
            else if(token.value == "==") {
                op->operator_type = OperatorType::EQ;
            }
            else if(token.value == "<") {
                op->operator_type = OperatorType::GREATER;
            }
            else if(token.value == "*") {
                op->operator_type = OperatorType::MUL;
            }
            else if(token.value == ">") {
                op->operator_type = OperatorType::LESS;
            }
            else if(token.value == "%") {
                op->operator_type = OperatorType::MODULO;
            }
            else if(token.value == "&&") {
                op->operator_type = OperatorType::AND;
            }
            else if(token.value == "||") {
                op->operator_type = OperatorType::OR;
            }

            else if(token.value == ".") {
                op->operator_type = OperatorType::DOT;
            }
            else if(token.value == "--") {
                op->operator_type = OperatorType::DECREMENT;
                op->is_one_arg = true;
            }
            else if(token.value == "++") {
                op->operator_type = OperatorType::INCREMENT;
                op->is_one_arg = true;
            }
            else if(token.value == "&&") {
                op->operator_type = OperatorType::ADD;
            }
            else if(token.value == "||") {
                op->operator_type = OperatorType::OR;
            }
            else if(token.value == "!") {
                op->operator_type = OperatorType::NEGATE;
                op->is_one_arg = true;
            }
            else if( token.value == ";") {
                break_constructing = true;
                return;
            }
            else if( token.value == ",") {
                break_constructing = true;
                return;
            }

            op->l_arg = this->last_frag;
            if(!op->is_one_arg) {
                op->r_arg = next_fragment();
                if(op->r_arg == nullptr) {
                    op->r_arg = this->last_frag;
                }
                op->debug_value =  op->l_arg->debug_value + " " + token.value + " " + op->r_arg->debug_value;
            }
            else  {
                op->debug_value =  op->l_arg->debug_value;
            }
            this->last_frag = op;


        }
        else if(token.type == TokenType::BOOLEAN)
        {
            this->last_frag = std::make_shared<Value>(token.value);
            this->last_frag->debug_value = token.value;
        }
        else if(token.type == TokenType::VARIABLE)
        {
            auto var_name = token.value;

            if(this->tokens[this->pos].value == "(") {
                auto function_call = std::make_shared<FunctionCallFragment>();
                read_fragment();
                function_call->function_name = var_name;
                function_call->args = read_statement();
                this->last_frag = function_call;
            }
            else {
                this->last_frag = std::make_shared<VariableReferenceFragment>(token.value);
                this->last_frag->debug_value = token.value;
            }


        }
    }

}

Token ASTCreator::next(bool move_iter) {
    if(pos < tokens.size()) {
        Token tok = tokens[pos];
        if(move_iter) {
            pos++;
        }
        return tok;
    }
}

void ASTCreator::create_ast() {
    while(this->pos < tokens.size()) {
        auto statement = read_statement();
        this->last_frag = nullptr;
        if(statement != nullptr) {
            this->as_tree.push_back(statement);
        }

    }
}

Statement *ASTCreator::read_statement() {

    auto statement = new Statement();

    this->last_statement = current_statement;
    this->break_constructing = false;
    this->current_statement = statement;



    if(this->semaphore_frag != nullptr) {
        if(this->semaphore_frag->type == FragmentType::BRACKET_OPEN) {
            this->semaphore_frag.reset();
            this->semaphore_frag = nullptr;

            auto next_statement = read_statement();
            while(next_statement != nullptr) {
                statement->composed_statements.push_back(next_statement);
                next_statement = read_statement();
            }

        }
        //statement group
        else if(semaphore_frag->type == FragmentType::CURLY_BRACKET_OPEN ) {
            this->semaphore_frag.reset();
            this->semaphore_frag = nullptr;

            auto next_statement = read_statement();
            while(next_statement != nullptr) {
                statement->composed_statements.push_back(next_statement);
                next_statement = read_statement();
            }
        }
        //array
        else if(this->semaphore_frag->type == FragmentType::SQUARE_BRACKET_OPEN) {
            this->semaphore_frag.reset();
            this->semaphore_frag = nullptr;

            statement->type = StatementType::ARRAY_STATEMENT;

            auto next_statement = read_statement();
            while(next_statement != nullptr) {
                statement->composed_statements.push_back(next_statement);
                next_statement = read_statement();
            }
        }
        else if (
                semaphore_frag->type == FragmentType::CURLY_BRACKET_CLOSE ||
                semaphore_frag->type == FragmentType::SQUARE_BRACKET_CLOSE ||
                semaphore_frag->type == FragmentType::BRACKET_CLOSE
                )
        {
            this->semaphore_frag.reset();
            this->semaphore_frag = nullptr;
            return nullptr;
        }
    }
    else {
        read_fragment();
        if(this->last_frag == nullptr) {
            return nullptr;
        }
        statement->ast_root = this->last_frag;
        this->last_frag = nullptr;
    }

    return statement;
}

std::shared_ptr<Fragment> ASTCreator::next_fragment() {
    read_fragment();
    return this->last_frag;
}

void ASTCreator::early_rollback() {
    delete current_statement;

    for(auto & element : as_tree) {
        delete element;
    }



}

VariableReferenceFragment::VariableReferenceFragment(const std::string &name) {
    this->type = FragmentType::VARIABLE;
    this->name = name;
}

Variable::Variable(ValueType type, std::shared_ptr<Value> value_ptr, std::string name) {
    this->value = value_ptr;
    this->name = std::move(name);
}

Statement::~Statement() {
//    delete ast_root;
//    delete condition;
//    delete on_init;
//    delete each;
    //delete child_statement;
    for(auto & element : composed_statements) {
        delete element;
    }
}

OperatorFragment::~OperatorFragment() {
    r_arg.reset();
    l_arg.reset();
}

KeywordFragment::~KeywordFragment() {
//    for(auto & element : arguments) {
//        delete element;
//    }
}

ASTError::ASTError(const std::string &message, int line, bool isCritical) : message(message), line(line),
                                                                            is_critical(isCritical) {}
