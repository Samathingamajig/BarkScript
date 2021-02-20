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
    bool isAlpha(char c);
    bool isUnderscore(char c);
    bool isIdentifierStarter(char c);
    bool isIdentifierCharacter(char c);
    SingleLexResult nextToken();

    MultiLexResult tokenize();
};

#endif // !LEXER_H
