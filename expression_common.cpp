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


void SEQL::ASTCreator::raise_error()
{
    this->break_constructing = true;
    this->state = ASTState::BROKEN;

    std::cout << "Line number: " << this->current_error.line << " -> "  << this->current_error.message << std::endl;
}

void SEQL::ASTCreator::read_fragment() {

    
    if(pos >= tokens.size())
    {
        break_constructing = true;
        this->last_frag = nullptr;
        return;
    }

    while (pos < tokens.size() && !break_constructing) {
        Token token = next();
        //primitives
        if(token.type == TokenType::NUMBER) {
            this->last_frag = new Value(token.value);
            this->last_frag->debug_value = token.value;
        }
        else if(token.type == TokenType::STRING) {
            this->last_frag = new Value(token.value);
            this->last_frag->debug_value = token.value;
        }
        else if(token.type == TokenType::KEYWORD){
            auto keyword_frag =  new KeywordFragment();
            if(token.value == "var") {
                Token var_tok = next();
                auto var_name = var_tok.value;
                keyword_frag->keyword_type = KeywordType::VAR;
                keyword_frag->arguments = { new Value(var_name) };
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
                auto frag = new KeywordFragment();
                frag->keyword_type = KeywordType::CONTINUE;

                this->last_frag = frag;
            }
            else if(token.value == "return") {
                auto return_frag = new KeywordFragment();
                read_fragment();
                auto last = this->last_frag;
                return_frag->keyword_type = KeywordType::RETURN;
                return_frag->arguments = {last};

                this->last_frag = return_frag;
            }
            else if(token.value == "fun") {
                auto function = new Function();
                
                auto frag = next_fragment();
                VariableReferenceFragment * function_name = nullptr;
                if(frag->type != FragmentType::VARIABLE)
                {
                    auto name = (VariableReferenceFragment*)(next_fragment());
                    
                    raise_error();
                    return;
                }   


                function->name = function_name->name;
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
                std::vector<Fragment*> for_fragments;
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

                    auto for_body = this->read_statement();
                    for_statement->child_statement = for_body;
                }
                else {
                    
                    this->current_error = ASTError("Failed to read for statement fragment was null", this->current_line, true);
                    this->break_constructing = true;
                    this->state = ASTState::BROKEN;
                    raise_error();
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
                semaphore_frag = new Fragment();
                semaphore_frag->type = FragmentType::CURLY_BRACKET_OPEN;
                return;
            }
            else if(token.value == "}") {
                break_constructing = true;
                semaphore_frag = new Fragment();
                semaphore_frag->type = FragmentType::CURLY_BRACKET_CLOSE;
                return;
            }
        }
        else if (token.type == TokenType::BRACKET) {

            if(token.value == "(") {
                break_constructing = true;
                semaphore_frag = new Fragment();
                semaphore_frag->type = FragmentType::BRACKET_OPEN;
                return;
            }

            else if(token.value == ")") {
                break_constructing = true;
                semaphore_frag = new Fragment();
                semaphore_frag->type = FragmentType::BRACKET_CLOSE;
                return;
            }

            if (token.value == "[") {
                //check if we use access operator, or start array
                if(this->last_frag != nullptr) {

                    bool isArrayValue = false;
                    if(this->last_frag->type == FragmentType::OPERATOR) {
                        auto oper = (OperatorFragment*)(this->last_frag);
                        isArrayValue = oper->operator_type == OperatorType::ARRAY_REFERENCE;
                    }

                    if(this->last_frag->type == FragmentType::VARIABLE || this->last_frag->type == FragmentType::STATEMENT_LINK || isArrayValue) {
                        auto array_operator = new OperatorFragment();
                        array_operator->operator_type  = OperatorType::ARRAY_REFERENCE;
                        array_operator->l_arg = this->last_frag;
                        array_operator->r_arg = this->next_fragment();
                        array_operator->debug_value = array_operator->l_arg->debug_value + array_operator->r_arg->debug_value;

                        this->semaphore_frag = nullptr;

                        this->last_frag = array_operator;
                        continue;
                    }
                }

                semaphore_frag = new Fragment();
                semaphore_frag->type = FragmentType::SQUARE_BRACKET_OPEN;
                auto array_statement = read_statement();
                auto link = new ArrayFragment(array_statement);
                this->last_frag = link;

                return;
            }
            else if(token.value == "]" && this->last_frag->type == FragmentType::VARIABLE || this->last_frag->type == FragmentType::VALUE ) {
                return;
            }
            else if (token.value == "]") {
                semaphore_frag = new Fragment();
                semaphore_frag->type = FragmentType::SQUARE_BRACKET_CLOSE;
                this->break_constructing = true;
                return;
            }
        }
        else if(token.type == TokenType::OPERATOR) {

            if( token.value == ";") {
                break_constructing = true;
                return;
            }
            else if( token.value == ",") {
                break_constructing = true;
                return;
            }

            auto binary_operators = std::map<std::string, OperatorType> {
                {"+",  OperatorType::ADD},
                {"+=",  OperatorType::PLUS_EQUAL},
                {"-=",  OperatorType::MINUS_EQUAL},
                {"=",  OperatorType::ASSIGN},
                {"==", OperatorType::EQ},
                {"<",  OperatorType::GREATER},
                {"*",  OperatorType::MUL},
                {">",  OperatorType::LESS},
                {"%",  OperatorType::MODULO},
                {"&&", OperatorType::AND},
                {"||", OperatorType::OR},
                {".",  OperatorType::DOT},
            };


            auto unary_operators = std::map<std::string, OperatorType> {
                {"--",  OperatorType::DECREMENT},
                {"++",  OperatorType::INCREMENT},
                {"!",  OperatorType::NEGATE},
            };

            auto op = new OperatorFragment();
            op->is_one_arg    = unary_operators.count(token.value) == 1;
            op->operator_type = op->is_one_arg ? unary_operators[token.value] : binary_operators[token.value]; 

            op->l_arg = this->last_frag;
            if(!op->is_one_arg) {
                op->r_arg = next_fragment();
                if(op->r_arg == nullptr) {
                    
                    this->state = ASTState::BROKEN;
                    
                    char err_message[512];
                    std::string snippet = op->l_arg->debug_value + " " + token.value;
                    snprintf(err_message, sizeof(err_message), "Failed to read next fragment for %s", snippet.c_str());
                    this->current_error = ASTError(
                        err_message, this->current_line, true
                    );

                    this->raise_error();
                }
                else
                {
                    op->debug_value =  op->l_arg->debug_value + " " + token.value + " " + op->r_arg->debug_value;
                }
            }
            else  {
                op->debug_value =  op->l_arg->debug_value;
            }
            this->last_frag = op;


        }
        else if(token.type == TokenType::BOOLEAN)
        {
            this->last_frag = new Value(token.value);
            this->last_frag->debug_value = token.value;
        }
        else if(token.type == TokenType::VARIABLE)
        {
            auto var_name = token.value;

            if(this->tokens[this->pos].value == "(") {
                auto function_call = new FunctionCallFragment();
                read_fragment();
                function_call->function_name = var_name;
                function_call->args = read_statement();
                this->last_frag = function_call;
            }
            else {
                this->last_frag = new VariableReferenceFragment(token.value);
                this->last_frag->debug_value = token.value;
            }
        }
    }

}

Token ASTCreator::next(bool move_iter) {
    if(pos < tokens.size()) {
        Token tok = tokens[pos];
        if(tok.value == "\n")
        {
            this->current_line++;
            pos++;
            return next(move_iter);
        }

        if(move_iter) 
            pos++;
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
            this->semaphore_frag = nullptr;

            auto next_statement = read_statement();
            while(next_statement != nullptr) {
                statement->composed_statements.push_back(next_statement);
                next_statement = read_statement();
            }

        }
        //statement group
        else if(semaphore_frag->type == FragmentType::CURLY_BRACKET_OPEN ) {
            this->semaphore_frag = nullptr;

            auto next_statement = read_statement();
            while(next_statement != nullptr) {
                statement->composed_statements.push_back(next_statement);
                next_statement = read_statement();
            }
        }
        //array
        else if(this->semaphore_frag->type == FragmentType::SQUARE_BRACKET_OPEN) {
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

Fragment * ASTCreator::next_fragment() {
    read_fragment();
    return this->last_frag;
}

void ASTCreator::rollback() {
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
    delete ast_root;
    delete condition;
    delete on_init;
    delete each;

    // child statement is within composed statements
    // if(child_statement != nullptr)
    // {
    //     delete child_statement;
    // }

    for(auto & element : composed_statements) {
        if(element != nullptr)
        {
            delete element;
        }
    }
}

OperatorFragment::~OperatorFragment() {
    delete r_arg;
    delete l_arg;
}

KeywordFragment::~KeywordFragment() {

}

ASTError::ASTError(const std::string &message, int line, bool isCritical) : message(message), line(line),
                                                                            is_critical(isCritical) {}
