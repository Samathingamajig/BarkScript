#pragma once
#ifndef ERROR_H
#define ERROR_H
#include <string>
#include "../position/position.h"
#include "../context/context.h"
#include "../vendor/strings_with_arrows/strings_with_arrows.h"

namespace errortypes {
    using namespace std;

    const string IllegalCharError = "IllegalCharError";
    const string InvalidSyntaxError = "InvalidSyntaxError";
    const string RuntimeError = "RuntimeError";
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

    Error(const Position& positionStart, const Position& positionEnd, const std::string& type, const std::string& details) {
        this->positionStart = positionStart;
        this->positionEnd = positionEnd;
        this->type = type;
        this->details = details;
        this->isError = true;
    }

    std::string virtual to_string() const {
        std::string output = type + ": " + details + '\n';
        output += "File \"" + positionStart.filename + "\", line " + std::to_string(positionStart.lineNumber + 1);
        output += "\n\n";
        output += strings_with_arrows(positionStart.filetext, positionStart, positionEnd);
        return output;
    }

    virtual operator spError() = 0;
    // All children should have the code below
    //operator spError() override {
    //    return makeSharedError(*this);
    //}
};

struct IllegalCharError : Error {
    IllegalCharError(const Position& positionStart, const Position& positionEnd, const std::string&& details) {
        this->positionStart = positionStart;
        this->positionEnd = positionEnd;
        this->type = errortypes::IllegalCharError;
        this->details = details;
        this->isError = true;
    }

    operator spError() override {
        return makeSharedError(*this);
    }
};

struct InvalidSyntaxError : Error {
    InvalidSyntaxError(const Position& positionStart, const Position& positionEnd, const std::string&& details) {
        this->positionStart = positionStart;
        this->positionEnd = positionEnd;
        this->type = errortypes::InvalidSyntaxError;
        this->details = details;
        this->isError = true;
    }

    operator spError() override {
        return makeSharedError(*this);
    }
};

struct RuntimeError : Error {
    spContext context;

    RuntimeError(const Position& positionStart, const Position& positionEnd, const std::string&& details, const spContext& context) {
        this->positionStart = positionStart;
        this->positionEnd = positionEnd;
        this->type = errortypes::RuntimeError;
        this->details = details;
        this->context = context;
    }

    std::string to_string() const override {
        std::string output = generateTraceback();
        output += type + ": " + details;
        output += "\n\n";
        output += strings_with_arrows(positionStart.filetext, positionStart, positionEnd);
        return output;
    }

    std::string generateTraceback() const {
        std::string output = "";
        const Position* pos = &positionStart;
        spContext ctx = context;

        while (ctx != nullptr) {
            output = "  File \"" + pos->filename + "\", line " + std::to_string(pos->lineNumber) + ", in \"" + ctx->displayName + "\"\n" + output;
            pos = &ctx->parentEntryPosition;
            ctx = ctx->parent;
        }

        return "Traceback (most recent call last):\n" + output;
    }

    operator spError() override {
        return makeSharedError(*this);
    }
};

#endif // !ERROR_H
