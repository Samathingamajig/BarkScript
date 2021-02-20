#pragma once
#ifndef TOKEN_H
#define TOKEN_H
#include <string>
#include "tokens.h"
#include "../position/position.h"

struct Token {
    std::string type;
    std::string value;
    Position positionStart;
    Position positionEnd;

    Token() {
        type = "UNDEFINED_TYPE";
        value = "NULL";
    }

    Token(std::string type, std::string value, Position positionStart, Position positionEnd) {
        this->type = type;
        this->value = value;
        this->positionStart = positionStart.copy();
        this->positionEnd = positionEnd.copy();
        this->positionEnd.advance();
    }

    std::string to_string() {
        return "Token(" + type + ", \"" + value + "\", " + std::to_string(positionStart.index) + ", " + std::to_string(positionEnd.index) + ")";
    }

    bool matches(std::string tokenType) {
        return this->type == tokenType;
    }

    bool matches(std::string tokenType, std::string value) {
        return this->type == tokenType && this->value == value;
    }
};

#endif // !TOKEN_H
