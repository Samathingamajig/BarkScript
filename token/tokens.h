#pragma once
#ifndef TOKENS_H
#define TOKENS_H

#include <string>

namespace tokens {
    using namespace std;

    // Math Symbols
    const string PLUS = "PLUS";
    const string MINUS = "MINUS";
    const string ASTERISK = "ASTERISK";
    const string F_SLASH = "F_SLASH";
    const string DOUBLE_ASTERISK = "DOUBLE_ASTERISK";
    const string DOUBLE_F_SLASH = "DOUBLE_F_SLASH";
    const string OPEN_PAREN = "OPEN_PAREN";
    const string CLOSE_PAREN = "CLOSE_PAREN";

    // Data types
    const string NUMBER = "NUMBER";

    // Control
    const string EEOF = "EOF";
    const string UNKNOWN = "UNKNOWN";
}

#endif // !TOKENS_H
