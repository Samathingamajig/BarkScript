#include "interpreter.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include "../ast/ast.h"
#include "../object/object.h"

bool RuntimeResult::hasError() { return error != nullptr; }

spObject RuntimeResult::registerRT(spObject object) {
    return object;
}

spObject RuntimeResult::registerRT(RuntimeResult rt) {
    if (rt.hasError()) {
        this->error = rt.error;
    }
    return rt.object;
}

RuntimeResult RuntimeResult::success(spObject object) {
    this->object = object;
    return *this;
}

RuntimeResult RuntimeResult::failure(spError error) {
    this->error = error;
    return *this;
}

RuntimeResult Interpreter::visit(spNode node, spContext context) {
    std::string type = node->nodeType;

    if (type == nodetypes::Number) {
        return visitNumberNode(node, context);
    } else if (type == nodetypes::VariableAssignment) {
        return visitVariableAssignmentNode(node, context);
    } else if (type == nodetypes::VariableRetrievement) {
        return visitVariableRetrievementNode(node, context);
    } else if (type == nodetypes::BinaryOperator) {
        return visitBinaryOperatorNode(node, context);
    } else if (type == nodetypes::UnaryOperator) {
        return visitUnaryOperatorNode(node, context);
    } else {
        return RuntimeResult().failure(makeSharedError(RuntimeError(node->positionStart, node->positionEnd, "Error: node type " + type + " does not have a visit method!", context)));
    }
}

RuntimeResult Interpreter::visitNumberNode(spNode node, spContext context) {
    spObject number = makeSharedObject(Number(node->token.value));
    number->setContext(context);
    number->setPosition(node->positionStart, node->positionEnd);
    return RuntimeResult().success(number);
}

RuntimeResult Interpreter::visitVariableAssignmentNode(spNode node, spContext context) {
    RuntimeResult rt;
    std::string variableName = node->token.value;
    spObject value = rt.registerRT(visit(node->valueNode, context));
    if (rt.hasError()) return rt;
    SymbolTableSetReturnCode success = context->symbolTable->set(variableName, value, true);
    switch (success) {
        case SymbolTableSetReturnCode::perfect: { return rt.success(value); }
        case SymbolTableSetReturnCode::errorGlobalConstantVariable: { return rt.failure(makeSharedError(RuntimeError(node->token.positionStart, node->positionEnd, "You cannot modify a global constant variable!", context))); }
        case SymbolTableSetReturnCode::errorUserDefinedConstantVariable: { return rt.failure(makeSharedError(RuntimeError(node->token.positionStart, node->positionEnd, "You cannot modify a constant variable!", context))); }
        case SymbolTableSetReturnCode::errorNotInScope: { return rt.failure(makeSharedError(RuntimeError(node->token.positionStart, node->positionEnd, "Variable " + variableName + " does not exist in the current scope!", context))); }
        default: { return rt.failure(makeSharedError(RuntimeError(node->token.positionStart, node->positionEnd, "Unknown return value when setting: " + std::to_string((int) success), context))); }
    }
}

RuntimeResult Interpreter::visitVariableRetrievementNode(spNode node, spContext context) {
    RuntimeResult rt;
    std::string variableName = node->token.value;
    spObject value = context->symbolTable->get(variableName);
    if (value == nullptr)
        return rt.failure(makeSharedError(RuntimeError(node->positionStart, node->positionEnd, "Variable \"" + variableName + "\" is not defined in the current scope!", context)));
    value = value->copy();
    value->setPosition(node->positionStart, node->positionEnd);
    return rt.success(value);
}

RuntimeResult Interpreter::visitBinaryOperatorNode(spNode node, spContext context) {
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

RuntimeResult Interpreter::visitUnaryOperatorNode(spNode node, spContext context) {
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
