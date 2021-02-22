#pragma once
#ifndef AST_H
#define AST_H
#include <string>
#include <memory>
#include "../token/token.h"
#include "../position/position.h"
#include "../symboltable/symboltable.h"

namespace nodetypes {
    using namespace std;

    const string Number = "NUMBER";
    const string VariableAssignment = "VARASS";
    const string VariableRetrievement = "VARRET";
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

    virtual operator spNode() = 0;
    // All children should have the code below
    //operator spNode() override {
    //    return makeSharedNode(*this);
    //}

    spNode leftNode;
    spNode rightNode;
    spNode valueNode;
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

    operator spNode() override {
        return makeSharedNode(*this);
    }
};

struct VariableAssignmentNode : Node {
    VariableAssignmentNode(Token token, spNode valueNode) {
        this->nodeType = nodetypes::VariableAssignment;
        this->token = token;
        this->valueNode = valueNode;
        this->positionStart = token.positionStart;
        this->positionEnd = valueNode->positionEnd;
    }

    std::string to_string() override {
        return token.value + " = " + valueNode->to_string();
    }

    operator spNode() override {
        return makeSharedNode(*this);
    }
};

struct VariableRetrievementNode : Node {
    VariableRetrievementNode(Token token) {
        this->nodeType = nodetypes::VariableRetrievement;
        this->token = token;
        this->positionStart = token.positionStart;
        this->positionEnd = token.positionEnd;
    }

    std::string to_string() override {
        return token.value;
    }

    operator spNode() override {
        return makeSharedNode(*this);
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

    operator spNode() override {
        return makeSharedNode(*this);
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

    operator spNode() override {
        return makeSharedNode(*this);
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

    operator spNode() override {
        return makeSharedNode(*this);
    }
};

#endif // !AST_H
