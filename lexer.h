#pragma once
#ifndef LEXER_H
#define LEXER_H
#include <string>
#include <vector>
#include "token.h"

struct Lexer {
    std::string input;
    int position = 0;
    int nextPosition = 1;
    char current;
    int lineCount = 0;
    int inputLength;
    bool finished = false;

    Lexer(std::string input);

    void readChar();
    char peekChar(int num);
    bool isNumeric(char c);
    Token nextToken();

    std::vector<Token> tokenize();
};

#endif // !LEXER_H
