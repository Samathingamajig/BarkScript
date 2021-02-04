#include "lexer.h"
#include "tokens.h"

Lexer::Lexer(std::string input) {
    this->input = input;
    this->inputLength = input.length();
    if (inputLength > 0) {
        this->current = input[0];
    } else {
        this->current = 0;
    }

    // Constant starters
    this->position = 0;
    this->nextPosition = 1;
    this->lineCount = 0;
    this->finished = false;
}

void Lexer::readChar() {
    if (nextPosition >= inputLength) {
        current = 0;
    } else {
        current = input[nextPosition];
    }
    position = nextPosition;
    nextPosition++;
}

char Lexer::peekChar(int num = 0) {
    if (nextPosition + num >= inputLength) {
        return 0;
    } else {
        return input[nextPosition + num];
    }
}

Token Lexer::nextToken() {
    Token token;
    switch (current) {
        case '+':
            {
                token = Token(tokens::PLUS, "+", position, position, lineCount);
                break;
            }
        case '\0':
            {
                finished = true;
                return Token(tokens::EEOF, "", position, position, lineCount);
            }
        default:
            {
                if (isNumeric(current)) {
                    token = Token(tokens::INT, std::string(1, current), position, position, lineCount);
                    while (isNumeric(peekChar())) {
                        token.value += peekChar();
                        readChar();
                    }
                    break;
                } else {
                    token = Token(tokens::UNKNOWN, std::string(1, current), position, position, lineCount);
                }
            }
    }
    readChar();
    return token;
}

bool Lexer::isNumeric(char c) {
    return 48 <= c && c <= 57;
}