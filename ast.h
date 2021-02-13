#pragma once
#ifndef AST_H
#define AST_H
#include <string>
#include "token.h"

namespace nodetypes {
    using namespace std;

    const string Number = "NUMBER";
    const string BinaryOperator = "BINOP";
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
    std::string virtual to_string() {
        return "Not implemented! " + nodeType;
    };

    Node() {}


    spNode leftNode;
    spNode rightNode;
};

struct NumberNode : Node {
    NumberNode(Token token) {
        this->nodeType = "NUMBER";
        this->token = token;
    }

    std::string to_string() override {
        return token.value;
    }
};

struct BinaryOperatorNode : Node {
    BinaryOperatorNode(spNode leftNode, Token token, spNode rightNode) {
        this->nodeType = "BINOP";
        this->token = token;
        this->leftNode = leftNode;
        this->rightNode = rightNode;
    }

    std::string to_string() override {
        return "(" + leftNode->to_string() + ", " + token.value + ", " + rightNode->to_string() + ")";
    }
};

struct UnaryOperatorNode : Node {
    UnaryOperatorNode(Token token, spNode rightNode) {
        this->nodeType = "UNOP";
        this->token = token;
        this->rightNode = rightNode;
    }

    std::string to_string() override {
        return "(" + token.value + ", " + rightNode->to_string() + ")";
    }
};

struct ErrorNode : Node {
    ErrorNode(Token token) {
        this->nodeType = "ERROR";
        this->token = token;
    }

    std::string to_string() override {
        return token.value;
    }
};

#endif // !AST_H
