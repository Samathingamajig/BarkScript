#pragma once
#ifndef PARSER_H
#define PARSER_H
#include <vector>
#include <memory>
#include <functional>
#include "../token/token.h"
#include "../ast/ast.h"
#include "../error/error.h"

struct ParseResult {
    spError error = nullptr;
    spNode node = nullptr;
    int advancementCount = 0;

    bool hasError() { return error != nullptr; }

    void registerAdvancement() {
        this->advancementCount++;
    }

    std::shared_ptr<Node> registerPR(ParseResult pr) {
        this->advancementCount += pr.advancementCount;
        if (pr.hasError()) {
            this->error = pr.error;
        }
        return pr.node;
    }

    ParseResult success(spNode node) {
        this->node = node;
        return *this;
    }

    ParseResult failure(spError error) {
        if (this->error == nullptr || this->advancementCount == 0)
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

    ParseResult atom();
    ParseResult exponent();
    ParseResult factor();
    ParseResult term();
    ParseResult expr();
    ParseResult parse();

    ParseResult binaryOperation(std::function<ParseResult()> rule, std::vector<std::string> allowedTokens);
    ParseResult binaryOperation(std::function<ParseResult()> rule1, std::vector<std::string> allowedTokens, std::function<ParseResult()> rule2);
};

#endif // !PARSER_H
