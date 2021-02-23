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

    bool hasError() const;
    spObject registerRT(const spObject& object);
    spObject registerRT(const RuntimeResult& rt);

    RuntimeResult success(const spObject& object);
    RuntimeResult failure(const spError& error);
};

struct Interpreter {
    RuntimeResult visit(const spNode& node, const spContext& context);

    RuntimeResult visitNumberNode(const spNode& node, const spContext& context);
    RuntimeResult visitVariableDeclarationNode(const spNode& node, const spContext& context);
    RuntimeResult visitVariableAssignmentNode(const spNode& node, const spContext& context);
    RuntimeResult visitVariableRetrievementNode(const spNode& node, const spContext& context);
    RuntimeResult visitBinaryOperatorNode(const spNode& node, const spContext& context);
    RuntimeResult visitUnaryOperatorNode(const spNode& node, const spContext& context);
};

#endif // !INTERPRETER_H
