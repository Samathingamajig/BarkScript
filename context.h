#pragma once
#ifndef CONTEXT_H
#define CONTEXT_H
#include <memory>
#include <string>
#include "position.h"

struct Context;

typedef std::shared_ptr<Context> spContext;

struct Context {
    std::string displayName = "UNKNOWN_CONTEXT_NAME";
    spContext parent;
    Position parentEntryPosition;

    Context(std::string displayName, spContext parent = nullptr, Position parentEntryPosition = Position()) {
        this->displayName = displayName;
        this->parent = parent;
        this->parentEntryPosition = parentEntryPosition;
    }
};

#endif // !CONTEXT_H
