#include "lexer.h"
#include <vector>
#include <memory>
#include "tokens.h"
#include "position.h"
#include "error.h"

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
                token = Token(tokens::ASTERISK, "*", position, position);
                break;
            }
        case '/':
            {
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
                    while (isNumeric(peekChar())) {
                        value += peekChar();
                        readChar();
                    }
                    token = Token(tokens::NUMBER, value, positionStart, position);
                    break;
                } else {
                    Position positionStart = position.copy();
                    char tempCurrent = current;
                    readChar();
                    return Error(positionStart, position, errortypes::IllegalCharError, std::string(1, '\'') + tempCurrent + "'");
                }
            }
    }
    readChar();
    return token;
}

bool Lexer::isNumeric(char c) {
    return 48 <= c && c <= 57;
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
