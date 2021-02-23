#pragma once
#ifndef LEXER_H
#define LEXER_H
#include <string>
#include <vector>
#include "../token/token.h"
#include "../error/error.h"

struct SingleLexResult {
    Token token;
    spError error = nullptr;

    SingleLexResult(const Token& token) {
        this->token = token;
    }

    SingleLexResult(const spError& error) {
        this->error = error;
    }
};

struct MultiLexResult {
    std::vector<Token> tokenized;
    spError error = nullptr;

    bool hasError() const { return error != nullptr; }

    MultiLexResult(const std::vector<Token>& tokenized) {
        this->tokenized = tokenized;
    }

    MultiLexResult(const spError& error) {
        this->error = error;
    }
};

struct Lexer {
    std::string input;
    Position position;
    char current;
    int lineCount = 0;
    int inputLength;
    bool finished = false;

    Lexer(const std::string& input, const std::string&& filename = "<stdin>");

    void readChar();
    char peekChar(const int& num = 0) const;
    bool isNumeric(const char& c) const;
    bool isAlpha(const char& c) const;
    bool isUnderscore(const char& c) const;
    bool isIdentifierStarter(const char& c) const;
    bool isIdentifierCharacter(const char& c) const;
    SingleLexResult nextToken();

    MultiLexResult tokenize();
};

#endif // !LEXER_H
