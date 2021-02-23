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

    bool hasError() const { return error != nullptr; }

    void registerAdvancement() {
        this->advancementCount++;
    }

    std::shared_ptr<Node> registerPR(const ParseResult& pr) {
        this->advancementCount += pr.advancementCount;
        if (pr.hasError()) {
            this->error = pr.error;
        }
        return pr.node;
    }

    ParseResult success(const spNode& node) {
        this->node = node;
        return *this;
    }

    ParseResult failure(const spError& error) {
        if (this->error == nullptr || this->advancementCount == 0)
            this->error = error;
        return *this;
    }
};

struct Parser {
    Parser(const std::vector<Token>& tokens);

    std::vector<Token> tokens;
    Token currentToken;
    int tokenIndex;

    Token nextToken();
    Token peekToken(unsigned int index = 0U) const;

    ParseResult atom();
    ParseResult exponent();
    ParseResult factor();
    ParseResult term();
    ParseResult assignment();
    ParseResult declaration();
    ParseResult expr();
    ParseResult statement();
    ParseResult parse();

    ParseResult binaryOperation(const std::function<ParseResult()>& rule, const std::vector<std::string>& allowedTokens);
    ParseResult binaryOperation(const std::function<ParseResult()>& rule1, const std::vector<std::string>& allowedTokens, const std::function<ParseResult()>& rule2);

    bool nextIsAssignment() const;
};

#endif // !PARSER_H
