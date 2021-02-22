#include "lexer.h"
#include <vector>
#include <memory>
#include "../token/tokens.h"
#include "../position/position.h"
#include "../error/error.h"
#include "../reservedwords/reservedwords.h"

Lexer::Lexer(std::string input, std::string filename) {
    this->input = input;
    this->inputLength = input.length();
    if (inputLength > 0) {
        this->current = input[0];
    } else {
        this->current = 0;
    }

    this->position = Position(-1, 0, -1, filename, input);
    this->position.advance();
    this->finished = false;
}

void Lexer::readChar() {
    if (position.index + 1 >= inputLength) {
        current = 0;
    } else {
        current = input[position.index + 1];
    }
    position.advance();
}

char Lexer::peekChar(int num = 0) {
    if (position.index + num + 1 >= inputLength) {
        return 0;
    } else {
        return input[position.index + num + 1];
    }
}

SingleLexResult Lexer::nextToken() {
    Token token;
reset:
    switch (current) {
        case '+':
        {
            token = Token(tokens::PLUS, "+", position, position);
            break;
        }
        case '-':
        {
            token = Token(tokens::MINUS, "-", position, position);
            break;
        }
        case '*':
        {
            if (peekChar() == '*') {
                Position start = position.copy();
                readChar();
                token = Token(tokens::DOUBLE_ASTERISK, "**", start, position);
                break;
            }
            token = Token(tokens::ASTERISK, "*", position, position);
            break;
        }
        case '/':
        {
            if (peekChar() == '/') {
                Position start = position.copy();
                readChar();
                token = Token(tokens::DOUBLE_F_SLASH, "//", start, position);
                break;
            }
            token = Token(tokens::F_SLASH, "/", position, position);
            break;
        }
        case '(':
        {
            token = Token(tokens::OPEN_PAREN, "(", position, position);
            break;
        }
        case ')':
        {
            token = Token(tokens::CLOSE_PAREN, ")", position, position);
            break;
        }
        case '=':
        {
            token = Token(tokens::EQUAL, "=", position, position);
            break;
        }
        case ' ':
        {
            readChar();
            goto reset;
        }
        case '\0':
        {
            finished = true;
            return Token(tokens::EEOF, "", position, position);
        }
        default:
        {
            if (isNumeric(current)) {
                std::string value = std::string(1, current);
                Position positionStart = position.copy();
                bool period = false;
                while (isNumeric(peekChar()) || (peekChar() == '.' && !period)) {
                    value += peekChar();
                    if (peekChar() == '.') {
                        period = true;
                    }
                    readChar();
                }
                token = Token(tokens::NUMBER, value, positionStart, position);
                break;
            } else if (isIdentifierStarter(current)) {
                std::string value = std::string(1, current);
                Position positionStart = position.copy();
                while (isIdentifierCharacter(peekChar())) {
                    value += peekChar();
                    readChar();
                }
                token = Token(isReservedWord(value) ? tokens::KEYWORD : tokens::IDENTIFIER, value, positionStart, position);
                break;
            } else {
                Position positionStart = position.copy();
                char tempCurrent = current;
                readChar();
                return (spError) IllegalCharError(positionStart, position, std::string(1, '\'') + tempCurrent + "'");
            }
        }
    }
    readChar();
    return token;
}

bool Lexer::isNumeric(char c) {
    return '0' <= c && c <= '9';
}

bool Lexer::isAlpha(char c) {
    return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

bool Lexer::isUnderscore(char c) {
    return c == '_';
}

bool Lexer::isIdentifierStarter(char c) {
    return isAlpha(c) || isUnderscore(c);
}

bool Lexer::isIdentifierCharacter(char c) {
    return isIdentifierStarter(c) || isNumeric(c);
}

MultiLexResult Lexer::tokenize() {
    std::vector<Token> tokenized;
    while (!finished) {
        SingleLexResult slr = nextToken();
        if (slr.error) {
            return slr.error;
        }
        tokenized.push_back(slr.token);
    }
    return tokenized;
}
