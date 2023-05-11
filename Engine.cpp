//
// Created by pnsv0 on 04.12.2022.
//

#include "Engine.h"

void SEQL::Engine::execute_file(const std::string& path) {
    this->lexer->tokenize_file(path);

    this->ast_creator->tokens = this->lexer->tokens;
    this->ast_creator->create_ast();
    this->functions = this->ast_creator->declared_functions;
    std::cout << "Finished creating AST tree..." << std::endl;

    auto statement = new Statement();
    statement->is_composed = true;
    statement->composed_statements = this->ast_creator->as_tree;
    this->execute_statement(statement);

    delete this->ast_creator;
}

void SEQL::Engine::execute_statement(Statement * statement) {
    if(statement == nullptr) return;

    for(const auto & element : statement->composed_statements) {
        if(element->type == StatementType::FOR_STATEMENT) {
            for (eval(element->on_init)
            ; evals_to_true(element->condition)
            ; eval(element->each) )
            {
                if(break_requested) {
                    break_requested = false;
                    break;
                }
                else if(continue_requested) {
                    continue_requested = false;
                    continue;
                }
                execute_statement(element->child_statement);
            }
        }
        else if(break_requested || continue_requested) {
            return;
        }
        else if(element->type == StatementType::ARRAY_STATEMENT) {
            for(const auto & sub_statement : element->child_statement->composed_statements) {
                this->execute_statement(sub_statement);
            }
        }
        else if(element->type == StatementType::IF_STATEMENT) {
            if(evals_to_true(element->condition)) {
                execute_statement(element->child_statement);
            }
            else if(element->continued_statement != nullptr) {
                //"else statement"
                if(element->continued_statement == nullptr) {
                    execute_statement(element);
                }
                //"else if statement"
                else if(evals_to_true((element->continued_statement->condition))) {
                    execute_statement(element->continued_statement);
                }
            }
        }
        else {
            this->eval(element->ast_root);
        }



    }
}


std::shared_ptr<SEQL::Value> SEQL::Engine::eval(std::shared_ptr<Fragment> fragment) {
    if(fragment->type == FragmentType::OPERATOR) {
        return handle_operator( std::static_pointer_cast<OperatorFragment>(fragment));
    }
    else if(fragment->type == FragmentType::KEYWORD) {
        return handle_keyword(std::static_pointer_cast<KeywordFragment>(fragment));
    }
    else if(fragment->type == FragmentType::STATEMENT_LINK) {
        auto link = std::static_pointer_cast<ArrayFragment>(fragment);
        auto array_value =  std::make_shared<Value>();
        array_value->array_statement = link->statement;
        array_value->value_type = ValueType::ARRAY;

        array_value->array_statement = link->statement;
        if(link->statement != nullptr) {
            if(array_value->array_values.empty()) {
                for(const auto & element : link->statement->composed_statements) {
                    array_value->array_values.push_back(this->eval(element->ast_root));
                }
            }

        }
        return array_value;
    }
    else if(fragment->type == FragmentType::FUNCTION_CALL) {
        auto function_call = std::static_pointer_cast<FunctionCallFragment>(fragment);
        auto fun = this->functions[function_call->function_name];

        auto args =  fun->function_args->composed_statements;
        auto vals = function_call->args->composed_statements;

        for(int i = 0 ; i < args.size(); i++) {
            auto var_frag = std::static_pointer_cast<VariableReferenceFragment>(args[i]->ast_root);
            auto val = eval(vals[i]->ast_root);

            auto var =  std::make_shared<Variable>();
            var->value = val;
            this->variables[var_frag->name] = var;

        }
        this->execute_statement(fun->function_body);

    }
    else if(fragment->type == FragmentType::VARIABLE) {
        auto variable = std::static_pointer_cast<VariableReferenceFragment>(fragment);
        auto var_value = this->variables[variable->name]->value;
        return var_value;
    }
    else if(fragment->type == FragmentType::VALUE) {
        return std::static_pointer_cast<Value>(fragment);
    }

    //non matched
    return nullptr;
}

bool SEQL::Engine::evals_to_true(std::shared_ptr<Fragment> condition_fragment) {
    auto value = (this->eval(condition_fragment));
    if(value != nullptr && value->result == "true") {
        return true;
    }
    return false;
}

