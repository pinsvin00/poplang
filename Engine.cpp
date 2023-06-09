//
// Created by pnsv0 on 04.12.2022.
//

#include "Engine.h"
#define NATIVE_TEMPLATED_FUNCTION(func) (           \
    {                                               \
        Function* f = new Function();               \
        f->is_native = true;                        \
        f->native_templated_func = [](std::vector<Value*> vals, Engine * self){\
            return self->func(vals);                                           \
        };                                                                     \
        f;                                                                     \
    })  

void SEQL::Scope::drop_local_variables()
{
    // for(auto & element : this->local_variables)
    // {
    //     delete element.second;
    //     element.second = nullptr;
    // }
}

std::string SEQL::Engine::stringifyValue(Value * value) {
    return str(value)->result;
}

void SEQL::Engine::execute_file(const std::string& path) {
    this->lexer->tokenize_file(path);

    this->ast_creator->tokens = this->lexer->tokens;
    this->ast_creator->create_ast();

    if(this->ast_creator->state == ASTState::BROKEN)
    {
        delete this->ast_creator;
        return;
    }

    this->functions = this->ast_creator->declared_functions;
    
    #ifdef DEBUG_LOGGING
    std::cout << "Finished creating AST tree..." << std::endl;
    #endif

    auto statement = new Statement();
    statement->is_composed = true;
    statement->composed_statements = this->ast_creator->as_tree;

    this->load_default_functions();
    this->scopes.push_back(new Scope());
    this->execute_statement(statement);
    drop_last_scope();

    delete this->ast_creator;
}


void SEQL::Engine::load_default_functions() 
{
    this->functions["int"] =        NATIVE_TEMPLATED_FUNCTION(to_int);
    this->functions["str"] =        NATIVE_TEMPLATED_FUNCTION(str);
    this->functions["typeof"] =     NATIVE_TEMPLATED_FUNCTION(type_of);
    this->functions["println"] =    NATIVE_TEMPLATED_FUNCTION(println);
    this->functions["print"] =      NATIVE_TEMPLATED_FUNCTION(print);
    this->functions["format"] =     NATIVE_TEMPLATED_FUNCTION(format);
}


void SEQL::Engine::execute_statement(Statement * statement) {
    if(statement == nullptr) return;

    for(const auto & element : statement->composed_statements) {
        if(element->type == StatementType::FOR_STATEMENT) {
            make_new_scope();
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
                make_new_scope();
                execute_statement(element->child_statement);
                drop_last_scope();
            }
            drop_last_scope();
        }
        else if(return_requested)
        {
            this->return_requested = false;
            continue;
        }
        //Give control to the for loop?
        else if(break_requested || continue_requested) 
        {
            return;
        }
        else if(element->type == StatementType::ARRAY_STATEMENT) 
        {
            for(const auto & sub_statement : element->child_statement->composed_statements) 
            {
                this->execute_statement(sub_statement);
            }
        }
        else if(element->type == StatementType::IF_STATEMENT) {
            if(evals_to_true(element->condition)) 
            {
                make_new_scope();
                execute_statement(element->child_statement);
                drop_last_scope();
                continue;
            }
            else if(element->continued_statement != nullptr) 
            {
                Statement * if_frag = element->continued_statement;
                while(if_frag != nullptr) 
                {
                    if(if_frag->type == StatementType::ELSE_STATEMENT)
                    {
                        make_new_scope();
                        execute_statement(if_frag->child_statement);
                        drop_last_scope();
                        break;
                    }
                    else if(
                         if_frag->type == StatementType::ELSE_IF_STATEMENT && 
                        evals_to_true((if_frag->condition))
                    )
                    {
                        make_new_scope();
                        execute_statement(if_frag->child_statement);
                        drop_last_scope();
                        break;
                    }
                    if_frag = if_frag->continued_statement;
                }
            }
        }
        else 
        {
            this->eval(element->ast_root);
        }
    }

    this->return_requested = false;
    this->break_requested = false;
    this->continue_requested = false;
}

void SEQL::Engine::make_new_scope() 
{
    this->scopes.push_back(new Scope());
}

void SEQL::Engine::drop_last_scope() 
{
    auto & last_scope = this->scopes[scopes.size()-1];
    last_scope->drop_local_variables();
    this->scopes.pop_back();

    std::vector<Value*> roots;
    for (int i = 0; i < scopes.size(); i++)
    {
        auto & element = scopes[i];
        for(auto & val : element->local_variables)
        {
            roots.push_back(val.second->value);
        }
    }
    
    this->gc->set_roots(roots);
    this->gc->run();
    
    //delete last_scope;
}


SEQL::Value* SEQL::Engine::eval(Fragment* fragment) {
    if(fragment->type == FragmentType::OPERATOR) 
    {
        return handle_operator( (OperatorFragment*)(fragment));
    }
    else if(fragment->type == FragmentType::KEYWORD)
    {
        return handle_keyword( (KeywordFragment*)(fragment));
    }
    else if(fragment->type == FragmentType::ARRAY) 
    {
        ArrayFragment* link = (ArrayFragment*)(fragment);
        Value* array_value = NEW_VALUE();
        array_value->array_statement = link->statement;
        array_value->value_type = ValueType::ARRAY;

        array_value->array_statement = link->statement;
        if(link->statement != nullptr) 
        {
            if(array_value->array_values == nullptr) 
            {
                array_value->array_values = new std::vector<Value*>();
                for(const auto & element : link->statement->composed_statements) 
                {
                    Value * evaluated = this->eval(element->ast_root);
                    array_value->array_values->push_back(evaluated);
                }
            }

        }
        else
        {
            sprintf(error.message, "Failed to resolve array."); 
            raise_error();
        }


        return array_value;
    }
    else if(fragment->type == FragmentType::PARENTHESES)
    {
        ParenthesesFragment* p_frag = (ParenthesesFragment*) fragment;
        return this->eval(p_frag->inner_frag);
    }
    else if (fragment->type == FragmentType::ARRAY_ACCESS) {
        ArrayAccessFragment * arr_access = (ArrayAccessFragment*) fragment;
        auto l = eval(arr_access->array_frag);
        auto r = eval(arr_access->index_expr->ast_root);

        auto index = bytes_to_int(r->result);
        auto deref = *l->array_values;
        auto result = deref[index];
        result->dispose = false;
        return result;
    }
    else if(fragment->type == FragmentType::DO_FRAGMENT)
    {
        auto do_frag = (DoFragment*) fragment;
        execute_statement(do_frag->stmt);
        if(this->stored_value != nullptr) {
            return this->stored_value;
        }
        else return NEW_VALUE("");
    }
    else if(fragment->type == FragmentType::FUNCTION_CALL) 
    {
        FunctionCallFragment * function_call = (FunctionCallFragment*)(fragment);
        Function * fun = this->functions[function_call->function_name];
        std::vector<Statement*> vals = function_call->args->composed_statements;

        if(fun->is_native)
        {   
            std::vector<Value*> evaluated;
            for(int i = 0 ; i < vals.size(); i++) 
            {
                Value * e = eval(vals[i]->ast_root);
                e->dispose = false;
                evaluated.push_back(e);
            }
            auto result = fun->native_templated_func(evaluated, this);
            // for(auto & element : evaluated)
            // {
            //     if(element->dispose){
            //         delete element;
            //     }

            // }
            return result;
        }
        else
        {
            auto args = fun->function_args->composed_statements;
            std::vector<Variable*> created_variables; 
            Scope * scope = new Scope();

            for(int i = 0 ; i < args.size(); i++) {
                auto var_frag = (VariableReferenceFragment*)(args[i]->ast_root);
                auto val = eval(vals[i]->ast_root);
                val->dispose = false;
                auto var = new Variable();

                var->value = val;
                scope->local_variables[var_frag->name] = var;

            }

            this->scopes.push_back(scope);
            this->execute_statement(fun->function_body);

            drop_last_scope();

            Value * result =  stored_value != nullptr ? stored_value : nullptr;
            stored_value = nullptr;
            return result;
        }
    }
    else if(fragment->type == FragmentType::VARIABLE) 
    {
        auto variable = (VariableReferenceFragment*)fragment;

        //reverse iterate through last scopes;
        for (int i = scopes.size() - 1; i >= 0; i--)
        {
            auto & element = scopes[i];
            if(element->local_variables.count(variable->name) != 0)
            {
                auto var_ref = element->local_variables[variable->name];
                auto var_value = var_ref->value;
                return var_value;
            }
        }

        this->error.is_critical = true;
        sprintf(this->error.message, "Failed to find variable with name %s \n", variable->name.c_str());
        raise_error();

    }
    else if(fragment->type == FragmentType::VALUE) 
    {
        //Duplicate value, very important!
        Value * new_value = NEW_VALUE((Value*)fragment);
        return new_value;
    }

    //non matched
    return nullptr;
}

bool SEQL::Engine::evals_to_true(Fragment * condition_fragment) 
{
    Value* value = this->eval(condition_fragment);
    if(value != nullptr && value->value_type == ValueType::BOOL && value->result[0] == 1) {
        return true;
    }
    return false;
}

