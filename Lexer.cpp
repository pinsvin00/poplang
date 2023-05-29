//
// Created by pnsv0 on 09.04.2022.
//

#include "Lexer.hpp"


void Lexer::tokenize_file(const std::string& path) {
    std::ifstream file (path);
    if(file.good()) {
        std::string buffer;
        while(std::getline(file, buffer)) {
            this->tokenize(buffer);
        }
    }
    else
    {
        std::cout << "Failed to load file at " << path << std::endl;
    }

}

void Lexer::tokenize(const std::string& line){
    std::string fragment_buffer;
    bool reading_string_literal = false;
    lines.push_back(line);
    tokens.push_back(Token("\n"));

    std::string operators = "()[]{}!+-*/%^&<>=;,. ";
    std::vector<std::string> char2Ops = {
            "++",
            "--",
            "+=",
            "-=",
            "&&",
            "||",
            "<=",
            ">=",
            "==",
    };

    if(line.size() > 2) {
        if(line[0] == '/' && line[1]) {
            return;
        }

    }

    for (size_t i = 0; i < line.size(); i++){
        //change reading of string literal
        if(line[i] == '\"') reading_string_literal = !reading_string_literal;
        if(!reading_string_literal) {
            if( i + 1 < line.size() ) {
                bool op2_found = false;
                std::string expectedOp;
                expectedOp += line[i];
                expectedOp += line[i+1];
                for(const auto & element : char2Ops) {
                    if(element == expectedOp) {
                        if(!fragment_buffer.empty() && fragment_buffer != " ") {
                            tokens.emplace_back(fragment_buffer);
                            fragment_buffer = "";
                        }
                        tokens.emplace_back(expectedOp);
                        op2_found = true;
                        i = i + 1;
                        break;
                    }
                }

                if(op2_found) {
                    continue;
                }
            }
            if(operators.find(line[i]) != std::string::npos) {
                if(!fragment_buffer.empty() && fragment_buffer != " ") {
                    tokens.emplace_back(fragment_buffer);
                    fragment_buffer = "";
                }

                if(line[i] != ' ') {
                    tokens.emplace_back(Token(std::string(1, line[i])));
                }
                fragment_buffer = "";
            }
            else {
                fragment_buffer += line[i];
            }
        }
        else {
            fragment_buffer += line[i];
        }
    }

    if(!fragment_buffer.empty()){
        auto fragment = Token(fragment_buffer);
        tokens.push_back(fragment);
    }

#ifdef DEBUG_LEXER
    for(auto & element : this->tokens)
    {
        std::cout << "Token: "<< element.value << " With type " << (int)element.type << std::endl;
    }
#endif
}

Token::Token(std::string val) {
    std::map<std::string, bool> boolean_values = {
            {"TRUE", true},
            {"FALSE", false},
    };

    if(boolean_values.count(val) != 0) {
        this->value = val;
        this->type = TokenType::BOOLEAN;
        return;
    }

    if(val == "NIL") {
        this->type = TokenType::NIL;
        this->value = "NIL";
        return;
    }

    if(val == "(" || val == ")") {
        this->type = TokenType::BRACKET;
        this->value = val;
        return;
    }
    if (val == "{" || val == "}") {
        this->type = TokenType::CURLY_BRACKET;
        this->value = val;
        return;
    }

    if(val[0] == '"') {
        this->type = TokenType::STRING;
        this->value = val.substr(1, val.length() - 2);
        return;
    }

    if(val == "[" || val == "]") {
        this->type = TokenType::BRACKET;
        this->value = val;
        return;
    }

    std::map<std::string, int> operator_priority = {
            {"=", 0}, {"==",0} , {"+", 1}, {"-", 1}, {"*", 2}, {"{", 0}, {"%", 0}, {"+=", 0}, {"-=", 0},
            {",", 3}, {";", 0}, {">", 0}, {"<", 0}, {"++" , 0} , {"--", 0}, {"&&", 0}, {"||", 0},
            {".", 0},
    };

    std::map<std::string, int> keyword_priority = {
            {"input", 0}, {"print", 0}, {"var", 0}, {"if", 0}, {"for", 0}, {"fun", 0}, {"return", 0},
            {"break", 0}, {"continue", 0}, {"else", 0},
    };

    std::map<std::string, int> db_native_func_priority = {
            {"INSERT_BEGIN", 0},
            {"INSERT", 0},
    };

    std::map<std::string, int> native_func_priority = {
            {"PRINT", 0},
    };

    if(operator_priority.count(val)) {
        this->type  = TokenType::OPERATOR;
        this->value = val;
        return;
    }

    if(keyword_priority.count(val)) {
        this->type = TokenType::KEYWORD;
        this->value = val;
        return;
    }
    bool is_number = isdigit(val[0]);
    bool is_string = val[0] == '\"';
    if(is_number || is_string) {
        if(is_number) {
            this->type = TokenType::NUMBER;
        }
        else {
            this->type = TokenType::STRING;
        }
        this->value = val;
        return;
    }

    this->type = TokenType::VARIABLE;
    this->value = val;
}
