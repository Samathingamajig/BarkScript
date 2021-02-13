#pragma once
#ifndef LEXER_H
#define LEXER_H
#include <string>
#include <vector>
#include "token.h"
#include "error.h"

struct SingleLexResult {
    Token token;
    spError error = nullptr;

    SingleLexResult(Token token) {
        this->token = token;
    }

    SingleLexResult(spError error) {
        this->error = error;
    }
};

struct MultiLexResult {
    std::vector<Token> tokenized;
    spError error = nullptr;

    bool hasError() { return error != nullptr; }

    MultiLexResult(std::vector<Token> tokenized) {
        this->tokenized = tokenized;
    }

    MultiLexResult(spError error) {
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

    Lexer(std::string input, std::string filename = "<stdin>");

    void readChar();
    char peekChar(int num);
    bool isNumeric(char c);
    SingleLexResult nextToken();

    MultiLexResult tokenize();
};

#endif // !LEXER_H
