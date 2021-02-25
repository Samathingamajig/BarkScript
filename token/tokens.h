#pragma once
#ifndef TOKENS_H
#define TOKENS_H

#include <string>

namespace tokens {
    using namespace std;

    // Math Symbols
    const string PLUS = "PLUS"; // +
    const string MINUS = "MINUS"; // -
    const string ASTERISK = "ASTERISK"; // *
    const string F_SLASH = "F_SLASH"; // /
    const string DOUBLE_ASTERISK = "DOUBLE_ASTERISK"; // **
    const string DOUBLE_F_SLASH = "DOUBLE_F_SLASH"; // //
    const string OPEN_PAREN = "OPEN_PAREN"; // (
    const string CLOSE_PAREN = "CLOSE_PAREN"; // )

    // Logical Operators
    const string BANG = "BANG"; // !

    // Comparison Operators
    const string DOUBLE_EQUAL = "DOUBLE_EQUAL"; // ==
    const string BANG_EQUAL = "BANG_EQUAL"; // !=
    const string LESS_THAN = "LESS_THAN"; // <
    const string LESS_THAN_EQUAL = "LESS_THAN_EQUAL"; // <=
    const string GREATER_THAN = "GREATER_THAN"; // >
    const string GREATER_THAN_EQUAL = "GREATER_THAN_EQUAL"; // >=

    // Keywords and Random Identifiers
    const string KEYWORD = "KEYWORD";
    const string IDENTIFIER = "IDENTIFIER";
    const string EQUAL = "EQUAL"; // =

    // Data types
    const string NUMBER = "NUMBER";

    // Control
    const string EEOF = "EOF"; // \0
    const string UNKNOWN = "UNKNOWN";
}

#endif // !TOKENS_H
