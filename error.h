#pragma once
#ifndef ERROR_H
#define ERROR_H
#include <string>
#include "position.h"
#include "strings_with_arrows.h"

namespace errortypes {
    using namespace std;

    const string IllegalCharError = "IllegalCharError";
    const string InvalidSyntaxError = "InvalidSyntaxError";
};

struct Error {
    Position positionStart;
    Position positionEnd;
    std::string type;
    std::string details;
    bool isError = false;

    operator bool() { return this->isError; }

    Error() {}

    Error(Position positionStart, Position positionEnd, std::string type, std::string details) {
        this->positionStart = positionStart;
        this->positionEnd = positionEnd;
        this->type = type;
        this->details = details;
        this->isError = true;
    }

    std::string to_string() {
        std::string output = type + ": " + details + '\n';
        output += "File " + positionStart.filename + ", line " + std::to_string(positionStart.lineNumber + 1);
        output += "\n\n";
        output += strings_with_arrows(positionStart.filetext, positionStart, positionEnd);
        return output;
    }
};

#endif // !ERROR_H

