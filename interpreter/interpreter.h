#pragma once
#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <memory>
#include "../ast/ast.h"
#include "../context/context.h"
#include "../error/error.h"

struct Object;
typedef std::shared_ptr<Object> spObject;

struct RuntimeResult {
    spError error = nullptr;
    spObject object = nullptr;

    bool hasError();
    spObject registerRT(spObject object);
    spObject registerRT(RuntimeResult rt);

    RuntimeResult success(spObject object);
    RuntimeResult failure(spError error);
};

struct Interpreter {
    RuntimeResult visit(spNode node, spContext context);

    RuntimeResult visitNumberNode(spNode node, spContext context);
    RuntimeResult visitVariableAssignmentNode(spNode node, spContext context);
    RuntimeResult visitVariableRetrievementNode(spNode node, spContext context);
    RuntimeResult visitBinaryOperatorNode(spNode node, spContext context);
    RuntimeResult visitUnaryOperatorNode(spNode node, spContext context);
};

#endif // !INTERPRETER_H
