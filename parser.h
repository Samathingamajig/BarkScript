#pragma once
#ifndef PARSER_H
#define PARSER_H
#include <vector>
#include <memory>
#include <functional>
#include "token.h"
#include "ast.h"

struct Parser {
    Parser(std::vector<Token> tokens);

    std::vector<Token> tokens;
    Token currentToken;
    int tokenIndex;

    Token nextToken();

    std::shared_ptr<Node> factor();
    std::shared_ptr<Node> term();
    std::shared_ptr<Node> expr();

    std::shared_ptr<Node> binaryOperation(std::function<std::shared_ptr<Node>()> rule, std::vector<std::string> allowedTokens);
};

#endif // !PARSER_H
