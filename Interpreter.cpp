#include "interpreter.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include "ast.h"
#include "object.h"

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
    } else if (type == nodetypes::BinaryOperator) {
        return visitBinaryOperatorNode(node, context);
    } else if (type == nodetypes::UnaryOperator) {
        return visitUnaryOperatorNode(node, context);
    } else {
        std::cout << "Error: node type " + type + " does not have a visit method!" << std::endl;
    }
}

RuntimeResult Interpreter::visitNumberNode(spNode node, spContext context) {
    spObject number = makeSharedObject(Number(node->token.value));
    number->setContext(context);
    number->setPosition(node->positionStart, node->positionEnd);
    return RuntimeResult().success(number);
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
