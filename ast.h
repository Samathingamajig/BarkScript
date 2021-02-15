#pragma once
#ifndef AST_H
#define AST_H
#include <string>
#include <memory>
#include "token.h"
#include "position.h"

namespace nodetypes {
    using namespace std;

    const string Number = "NUMBER";
    const string BinaryOperator = "BINOP";
    const string UnaryOperator = "UNOP";
    const string Error = "ERROR";
};

struct Node;

typedef std::shared_ptr<Node> spNode;

// Polymorphism without having to cast to unknown types later on
// https://stackoverflow.com/a/42539569/12101554
// https://ideone.com/4jdhfZ
template<class NodeType>
spNode makeSharedNode(NodeType&& node) {
    return std::make_shared<std::remove_reference_t<NodeType>>(std::forward<NodeType>(node));
}

struct Node {
    std::string nodeType;
    Token token;
    Position positionStart;
    Position positionEnd;

    std::string virtual to_string() {
        return "Not implemented! " + nodeType;
    };

    Node() {}

    spNode leftNode;
    spNode rightNode;
};

struct NumberNode : Node {
    NumberNode(Token token) {
        this->nodeType = nodetypes::Number;
        this->token = token;
        this->positionStart = token.positionStart;
        this->positionEnd = token.positionEnd;
    }

    std::string to_string() override {
        return token.value;
    }
};

struct BinaryOperatorNode : Node {
    BinaryOperatorNode(spNode leftNode, Token token, spNode rightNode) {
        this->nodeType = nodetypes::BinaryOperator;
        this->token = token;
        this->leftNode = leftNode;
        this->rightNode = rightNode;
        this->positionStart = leftNode->positionStart;
        this->positionEnd = rightNode->positionEnd;
    }

    std::string to_string() override {
        return "(" + leftNode->to_string() + ", " + token.type + ", " + rightNode->to_string() + ")";
    }
};

struct UnaryOperatorNode : Node {
    UnaryOperatorNode(Token token, spNode rightNode) {
        this->nodeType = nodetypes::UnaryOperator;
        this->token = token;
        this->rightNode = rightNode;
        this->positionStart = token.positionStart;
        this->positionEnd = rightNode->positionEnd;
    }

    std::string to_string() override {
        return "(" + token.type + ", " + rightNode->to_string() + ")";
    }
};

struct ErrorNode : Node {
    ErrorNode(Token token) {
        this->nodeType = nodetypes::Error;
        this->token = token;
    }

    std::string to_string() override {
        return token.value;
    }
};

#endif // !AST_H
