//
// Created by pnsv0 on 09.04.2022.
//

#ifndef POPDB_LEXER_H
#define POPDB_LEXER_H

#include <iostream>
#include <vector>
#include <map>
#include <fstream>

enum class TokenType {
    STRING,
    NUMBER,
    NIL,
    BOOLEAN,
    VARIABLE,
    KEYWORD,
    OPERATOR,
    CURLY_BRACKET,
    BRACKET,
    UNSPECIFIED,
};

class Token {
public:
    TokenType type = TokenType::UNSPECIFIED;
    std::string value;
    Token() = default;
    explicit Token(std::string val);
};

class Lexer {
public:
    unsigned int current_line = 1;
    std::vector<std::string> lines;
    std::vector<Token> tokens;
    void tokenize(const std::string& line);
    void tokenize_file(const std::string &path);
};



#endif //POPDB_LEXER_H