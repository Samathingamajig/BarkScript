#include "interpreter.h"
#include <iostream>
#include <string>
#include "../ast/ast.h"
#include "../object/object.h"

bool RuntimeResult::hasError() const { return error != nullptr; }

spObject RuntimeResult::registerRT(const spObject& object) {
    return object;
}

spObject RuntimeResult::registerRT(const RuntimeResult& rt) {
    if (rt.hasError()) {
        this->error = rt.error;
    }
    return rt.object;
}

RuntimeResult RuntimeResult::success(const spObject& object) {
    this->object = object;
    return *this;
}

RuntimeResult RuntimeResult::failure(const spError& error) {
    this->error = error;
    return *this;
}

RuntimeResult Interpreter::visit(const spNode& node, const spContext& context) {
    std::string type = node->nodeType;

    if (type == nodetypes::Number) {
        return visitNumberNode(node, context);
    } else if (type == nodetypes::VariableDeclaration) {
        return visitVariableDeclarationNode(node, context);
    } else if (type == nodetypes::VariableAssignment) {
        return visitVariableAssignmentNode(node, context);
    } else if (type == nodetypes::VariableRetrievement) {
        return visitVariableRetrievementNode(node, context);
    } else if (type == nodetypes::BinaryOperator) {
        return visitBinaryOperatorNode(node, context);
    } else if (type == nodetypes::UnaryOperator) {
        return visitUnaryOperatorNode(node, context);
    } else {
        return RuntimeResult().failure(RuntimeError(node->positionStart, node->positionEnd, "Error: node type " + type + " does not have a visit method!", context));
    }
}

RuntimeResult Interpreter::visitNumberNode(const spNode& node, const spContext& context) {
    spObject number = Number(node->token.value);
    number->setContext(context);
    number->setPosition(node->positionStart, node->positionEnd);
    return RuntimeResult().success(number);
}

RuntimeResult Interpreter::visitVariableDeclarationNode(const spNode& node, const spContext& context) {
    RuntimeResult rt;
    std::string variableName = node->token.value;
    if (context->symbolTable->exists(variableName, false)) {
        return rt.failure(RuntimeError(node->positionStart, node->positionEnd, "Variable " + variableName + " is already declared in the current scope!", context));
    }
    spObject value;
    value = rt.registerRT(visit(node->valueNode, context));
    if (rt.hasError()) return rt;
    SymbolTableSetReturnCode success = context->symbolTable->set(variableName, value, true);
    switch (success) {
        case SymbolTableSetReturnCode::perfect: { return rt.success(value); }
        case SymbolTableSetReturnCode::errorGlobalConstantVariable: { return rt.failure(RuntimeError(node->token.positionStart, node->positionEnd, "You cannot modify a global constant variable!", context)); }
        case SymbolTableSetReturnCode::errorUserDefinedConstantVariable: { return rt.failure(RuntimeError(node->token.positionStart, node->positionEnd, "You cannot modify a constant variable!", context)); }
        case SymbolTableSetReturnCode::errorNotInScope: { return rt.failure(RuntimeError(node->token.positionStart, node->positionEnd, "Variable \"" + variableName + "\" does not exist in the current scope!", context)); }
        default: { return rt.failure(RuntimeError(node->token.positionStart, node->positionEnd, "Unknown return value when setting: " + std::to_string((int) success), context)); }
    }
}

RuntimeResult Interpreter::visitVariableAssignmentNode(const spNode& node, const spContext& context) {
    RuntimeResult rt;
    std::string variableName = node->token.value;
    spObject value = rt.registerRT(visit(node->valueNode, context));
    if (rt.hasError()) return rt;
    SymbolTableSetReturnCode success = context->symbolTable->set(variableName, value, false);
    switch (success) {
        case SymbolTableSetReturnCode::perfect: { return rt.success(value); }
        case SymbolTableSetReturnCode::errorGlobalConstantVariable: { return rt.failure(RuntimeError(node->token.positionStart, node->positionEnd, "You cannot modify a global constant variable!", context)); }
        case SymbolTableSetReturnCode::errorUserDefinedConstantVariable: { return rt.failure(RuntimeError(node->token.positionStart, node->positionEnd, "You cannot modify a constant variable!", context)); }
        case SymbolTableSetReturnCode::errorNotInScope: { return rt.failure(RuntimeError(node->token.positionStart, node->positionEnd, "Variable \"" + variableName + "\" does not exist in the current scope!", context)); }
        default: { return rt.failure(RuntimeError(node->token.positionStart, node->positionEnd, "Unknown return value when setting: " + std::to_string((int) success), context)); }
    }
}

RuntimeResult Interpreter::visitVariableRetrievementNode(const spNode& node, const spContext& context) {
    RuntimeResult rt;
    std::string variableName = node->token.value;
    spObject value = context->symbolTable->get(variableName);
    if (value == nullptr)
        return rt.failure(RuntimeError(node->positionStart, node->positionEnd, "Variable \"" + variableName + "\" is not defined in the current scope!", context));
    value = value->copy();
    value->setPosition(node->positionStart, node->positionEnd);
    return rt.success(value);
}

RuntimeResult Interpreter::visitBinaryOperatorNode(const spNode& node, const spContext& context) {
    RuntimeResult rt;
    spObject left = rt.registerRT(visit(node->leftNode, context));
    if (rt.hasError()) return rt;
    spObject right = rt.registerRT(visit(node->rightNode, context));
    if (rt.hasError()) return rt;

    RuntimeResult result;

    std::string optoken = node->token.type;
    if (optoken == tokens::PLUS) {
        result = left->binary_plus(right);
    } else if (optoken == tokens::MINUS) {
        result = left->binary_minus(right);
    } else if (optoken == tokens::ASTERISK) {
        result = left->binary_asterisk(right);
    } else if (optoken == tokens::F_SLASH) {
        result = left->binary_f_slash(right);
    } else if (optoken == tokens::DOUBLE_ASTERISK) {
        result = left->binary_double_asterisk(right);
    } else if (optoken == tokens::DOUBLE_F_SLASH) {
        result = left->binary_double_f_slash(right);
    }

    if (result.hasError()) return rt.failure(result.error);

    result.object->setPosition(node->positionStart, node->positionEnd);
    return rt.success(result.object);
}

RuntimeResult Interpreter::visitUnaryOperatorNode(const spNode& node, const spContext& context) {
    RuntimeResult rt;
    spObject object = rt.registerRT(visit(node->rightNode, context));
    if (rt.hasError()) return rt;

    RuntimeResult result;

    std::string optoken = node->token.type;
    if (optoken == tokens::PLUS) {
        result = object->unary_plus();
    } else if (optoken == tokens::MINUS) {
        result = object->unary_minus();
    }

    if (result.hasError()) return rt.failure(result.error);

    result.object->setPosition(node->positionStart, node->positionEnd);
    return rt.success(result.object);
}
