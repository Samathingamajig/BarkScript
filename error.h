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
}

struct Error;

typedef std::shared_ptr<Error> spError;

// Polymorphism without having to cast to unknown types later on
// https://stackoverflow.com/a/42539569/12101554
// https://ideone.com/4jdhfZ
template<class ErrorType>
spError makeSharedError(ErrorType&& error) {
    return std::make_shared<std::remove_reference_t<ErrorType>>(std::forward<ErrorType>(error));
}

struct Error {
    Position positionStart;
    Position positionEnd;
    std::string type;
    std::string details;
    bool isError = false;

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

struct IllegalCharError : Error {
    IllegalCharError(Position positionStart, Position positionEnd, std::string details) {
        this->positionStart = positionStart;
        this->positionEnd = positionEnd;
        this->type = errortypes::IllegalCharError;
        this->details = details;
        this->isError = true;
    }
};

struct InvalidSyntaxError : Error {
    InvalidSyntaxError(Position positionStart, Position positionEnd, std::string details) {
        this->positionStart = positionStart;
        this->positionEnd = positionEnd;
        this->type = errortypes::InvalidSyntaxError;
        this->details = details;
        this->isError = true;
    }
};

#endif // !ERROR_H
