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
    this->finished_reading_stmt = true;
    this->state = ASTState::BROKEN;

    std::cout << "Line number: " << this->error.line << " -> "  << this->error.message << std::endl;
}

void SEQL::ASTCreator::read_fragment() {

    
    if(pos >= tokens.size())
    {
        finished_reading_stmt = true;
        this->last_frag = nullptr;
        return;
    }

    while (pos < tokens.size() && !finished_reading_stmt) {
        Token token = next();
        //primitives
        if(token.type == TokenType::NUMBER) {
            int32_t u = std::stoi(token.value.c_str());
            auto val = new Value(u);
            this->last_frag = val;
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
            else if(token.value == "if") {
                auto if_statement = this->current_statement;
                if_statement->is_composed = true;
                if_statement->composed_statements = {};

                read_fragment();
                auto condition_fragment = this->last_frag;
                new_reader_scope();
                auto composed_statement = this->read_statement();
                pop_reader_scope();

                //now we have to read the next statement
                if_statement->type = StatementType::IF_STATEMENT;
                if_statement->child_statement = composed_statement;
                if_statement->condition = condition_fragment;

                this->current_statement = if_statement;
                this->last_frag = keyword_frag;
                //no need to continue, since the statement has been fully read
                this->finished_reading_stmt = true;
                this->semaphore_frag = nullptr;

                return;

            }
            else if(token.value == "else") {
                //check if next token is if
                auto last_if = this->last_statement;
                auto else_statement = this->current_statement;
                else_statement->is_composed = true;
                else_statement->condition = nullptr;

                //read semaphore
                Token tok = next();

                if(tok.value == "{") 
                {
                    finished_reading_stmt = true;
                    semaphore_frag = new Fragment();
                    semaphore_frag->type = FragmentType::CURLY_BRACKET_OPEN;
                    else_statement->type = StatementType::ELSE_STATEMENT;
                }
                else if(tok.value == "if") 
                { 
                    if(last_if->type == StatementType::ELSE_IF_STATEMENT || last_if->type == StatementType::IF_STATEMENT) {
                        else_statement->type = StatementType::ELSE_IF_STATEMENT;
                        else_statement->condition = next_fragment();

                        //read_fragment();
                        if(semaphore_frag == nullptr || semaphore_frag->type != FragmentType::CURLY_BRACKET_OPEN) {
                            error.message = "Expected { token";
                            error.is_critical = true;    
                            raise_error();
                        }
                    }
                    else {
                        error.message = "Expected last statement to be if or else if";
                        error.is_critical = true;
                        raise_error();
                    }
                }

                //semaphore is certainly read
                new_reader_scope();
                auto composed_statement = this->read_statement();
                pop_reader_scope();

                //now we have to read the next statement
                else_statement->child_statement = composed_statement;
                last_if->continued_statement = else_statement;

                this->current_statement = else_statement;
                this->last_frag = keyword_frag;
                this->finished_reading_stmt = true;
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
            else if(token.value == "do")
            {
                auto frag = new DoFragment();
                read_fragment();
                frag->stmt = read_statement();
                this->last_frag = frag;
                continue;
            }
            else if(token.value == "return" ) {
                auto return_frag = new KeywordFragment();
                read_fragment();
                auto last = this->last_frag;
                return_frag->keyword_type = KeywordType::RETURN;
                return_frag->arguments = {last};
                keyword_frag = return_frag;
            }
            else if(token.value == "fun") {
                auto function = new Function();
                this->readingFunctionDeclaration = true;
                auto tok = next();
                if(tok.type != TokenType::VARIABLE)
                {
                     raise_error();
                     return;
                }

                function->name = tok.value;
                read_fragment();
                function->function_args = read_statement();
                read_fragment(); //read {
                if(semaphore_frag == nullptr || semaphore_frag->type != FragmentType::CURLY_BRACKET_OPEN)
                {
                    this->error.line = this->current_line;
                    this->error.message = "Expected '{' operator after function declaration";
                    delete function;
                    raise_error();
                    return;
                }
                this->readingFunctionDeclaration = false;
                function->function_body = read_statement();
                delete semaphore_frag;
                semaphore_frag = nullptr;

                this->declared_functions[function->name] = function;
                this->last_frag = nullptr;
                return;
            }
            else if(token.value == "for") {
                std::vector<Fragment*> for_fragments;
                for (int i = 0; i < 3; ++i) {
                    read_fragment();
                    for_fragments.push_back(this->last_frag);
                    finished_reading_stmt = false;
                }
                auto for_statement = this->current_statement;
                if(for_statement != nullptr && for_fragments.size() == 3) {
                    for_statement->is_composed = true;
                    for_statement->type = StatementType::FOR_STATEMENT;

                    for_statement->on_init = for_fragments[0];
                    for_statement->condition = for_fragments[1];
                    for_statement->each = for_fragments[2];
                    for_statement->is_composed = true;

                    new_reader_scope();
                    auto for_body = this->read_statement();
                    pop_reader_scope();

                    for_statement->child_statement = for_body;
                }
                else {
                    
                    this->error = ASTError("Failed to read for statement fragment was null", this->current_line, true);
                    this->finished_reading_stmt = true;
                    this->state = ASTState::BROKEN;
                    raise_error();
                    return;
                }

                this->last_frag = keyword_frag;
                this->finished_reading_stmt = true;
                this->semaphore_frag = nullptr;

                return;
            }
            this->last_frag = keyword_frag;
        }
        else if(token.type == TokenType::CURLY_BRACKET) {
            if(token.value == "{") {
                finished_reading_stmt = true;
                semaphore_frag = new Fragment();
                semaphore_frag->type = FragmentType::CURLY_BRACKET_OPEN;
                return;
            }
            else if(token.value == "}") {
                finished_reading_stmt = true;
                semaphore_frag = new Fragment();
                semaphore_frag->type = FragmentType::CURLY_BRACKET_CLOSE;
                return;
            }
        }
        else if(token.type == TokenType::BRACKET) {

            if(token.value == "(") {
                if(!readingFunctionDeclaration)
                {
                    Fragment* inner_fragment = next_fragment();
                    ParenthesesFragment* frag = new ParenthesesFragment(inner_fragment);
                    this->last_frag = frag;

                    this->finished_reading_stmt = false;
                    delete semaphore_frag;
                    semaphore_frag = nullptr;
                }
                else
                {
                    //reading arguments template
                    semaphore_frag = new Fragment();
                    semaphore_frag->type = FragmentType::BRACKET_OPEN;
                    return;
                }

            }

            else if(token.value == ")") {
                finished_reading_stmt = true;
                semaphore_frag = new Fragment();
                semaphore_frag->type = FragmentType::BRACKET_CLOSE;
                return;
            }

            if (token.value == "[") {
                //check if we use access operator, or start array
                if(this->last_frag != nullptr) {
                    if(
                        this->last_frag->type == FragmentType::VARIABLE ||
                        this->last_frag->type == FragmentType::ARRAY ||
                        this->last_frag->type == FragmentType::ARRAY_ACCESS
                    ) {
                        auto arr_access = new ArrayAccessFragment();
                        arr_access->array_frag = this->last_frag;

                        semaphore_frag = new Fragment();
                        semaphore_frag->type = FragmentType::SQUARE_BRACKET_OPEN;

                        new_reader_scope();
                        auto stmt = this->read_statement();
                        if(stmt->composed_statements.size() != 1)
                        {
                            error.message = "Index expression needs to consist of one sub statement";
                            error.is_critical = true;
                            raise_error();
                        }
                        pop_reader_scope();

                        arr_access->index_expr = stmt->composed_statements[0];
                        arr_access->index_expr->type = StatementType::NON_SPECIFIED;
                        arr_access->debug_value = arr_access->array_frag->debug_value + "[]";

                        this->semaphore_frag = nullptr;

                        this->last_frag = arr_access;
                        continue;
                    }
                }

                semaphore_frag = new Fragment();
                semaphore_frag->type = FragmentType::SQUARE_BRACKET_OPEN;
                auto array_statement = read_statement();
                auto link = new ArrayFragment(array_statement);
                //there maybe more stuff that has to be read
                this->finished_reading_stmt = false;
                this->last_frag = link;
            }
            else if(token.value == "]" && readingArrayRef) {
                //finished reading array_ref
                return;
            }
            else if (token.value == "]") {
                //finished reading array_stmt
                semaphore_frag = new Fragment();
                semaphore_frag->type = FragmentType::SQUARE_BRACKET_CLOSE;
                this->finished_reading_stmt = true;
                return;
            }
        }
        else if(token.type == TokenType::OPERATOR) {

            if( token.value == ";") {
                finished_reading_stmt = true;
                return;
            }
            else if( token.value == ",") {
                finished_reading_stmt = true;
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
                    this->error = ASTError(
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
            if(token.value == "true")
            {
                this->last_frag = new Value(true);
            }
            else if(token.value == "false")
            {
                this->last_frag = new Value(false);
            }
            else
            {
                this->error.message = "Invalid value for boolean";
                raise_error();
            }
            this->last_frag->debug_value = token.value;
        }
        else if(token.type == TokenType::VARIABLE)
        {
            auto var_name = token.value;

            if(this->tokens[this->pos].value == "(" && !readingFunctionDeclaration) {
                auto function_call = new FunctionCallFragment();
                //read '('
                this->readingFunctionDeclaration = true;
                read_fragment();
                this->readingFunctionDeclaration = false;
                function_call->function_name = var_name;
                new_reader_scope();
                function_call->args = read_statement();
                pop_reader_scope();
                this->last_frag = function_call;
            }
            else {
                this->last_frag = new VariableReferenceFragment(token.value);
                this->last_frag->debug_value = token.value;
            }
        }
    }

}

void ASTCreator::load_reader_scope(ASTScope* scope)
{
    this->last_statement = scope->current_statement;

    this->last_frag = scope->last_frag;

    this->readingArrayRef = scope->readingArrayRef;
    this->readingFunctionDeclaration = scope->readingFunctionDeclaration;
}



ASTScope * ASTCreator::new_reader_scope()
{
    ASTScope * scope = new ASTScope();
    this->scopes.emplace(scope);
    load_reader_scope(scope);

    return scope;
}

ASTScope * ASTCreator::pop_reader_scope()
{
    auto scope = scopes.top();
    scopes.pop();
    load_reader_scope(scopes.top());
    return scope;
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
    return Token();
}

void ASTCreator::create_ast() {
    new_reader_scope();
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
    statement->line_no = this->current_line;
    this->last_statement = current_statement;
    this->finished_reading_stmt = false;
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
            delete statement;

            this->current_statement = last_statement; 
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

Variable::Variable(ValueType type, Value* value_ptr, std::string name) {
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
