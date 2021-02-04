#pragma once
#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include "tokens.h"

struct Token {
    std::string type;
    std::string value;
    int positionStart;
    int positionEnd;
    int line;
    
    Token() {
        this->type = tokens::UNKNOWN;
        this->value = "";
        this->positionStart = 0;
        this->positionEnd = 0;
        this->line = 0;
    }

    Token(std::string type, std::string value, int positionStart, int positionEnd, int line) {
        this->type = type;
        this->value = value;
        this->positionStart = positionStart;
        this->positionEnd = positionEnd;
        this->line = line;
    }

    std::string to_string() {
        return "Token(" + type + ", \"" + value + "\", " + std::to_string(positionStart) + ", " + std::to_string(positionEnd) + ")";
    }
};

#endif // !TOKEN_H
