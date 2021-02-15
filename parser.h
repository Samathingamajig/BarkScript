#pragma once
#ifndef PARSER_H
#define PARSER_H
#include <vector>
#include <memory>
#include <functional>
#include "token.h"
#include "ast.h"
#include "error.h"

struct ParseResult {
    spError error = nullptr;
    spNode node;

    bool hasError() { return error != nullptr; }

    spNode registerPR(spNode node) {
        return node;
    }

    std::shared_ptr<Node> registerPR(ParseResult pr) {
        if (pr.hasError()) {
            this->error = pr.error;
        }
        return pr.node;
    }

    ParseResult success(std::shared_ptr<Node> node) {
        this->node = node;
        return *this;
    }

    ParseResult failure(spError error) {
        this->error = error;
        return *this;
    }
};

struct Parser {
    Parser(std::vector<Token> tokens);

    std::vector<Token> tokens;
    Token currentToken;
    int tokenIndex;

    Token nextToken();

    ParseResult factor();
    ParseResult term();
    ParseResult expr();
    ParseResult parse();

    ParseResult binaryOperation(std::function<ParseResult()> rule, std::vector<std::string> allowedTokens);
};

#endif // !PARSER_H
