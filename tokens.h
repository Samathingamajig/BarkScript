#pragma once
#ifndef TOKENS_H
#define TOKENS_H

#include <string>

namespace tokens {
    using namespace std;

    // Math Symbols
    const string PLUS = "+";
    const string MINUS = "-";
    const string ASTERISK = "*";
    const string F_SLASH = "/";
    const string OPEN_PAREN = "(";
    const string CLOSE_PAREN = ")";

    // Data types
    const string NUMBER = "NUMBER";

    // Control
    const string EEOF = "EOF";
    const string UNKNOWN = "UNKNOWN";
}

#endif // !TOKENS_H
