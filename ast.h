#pragma once
#ifndef AST_H
#include <string>
#include <iostream>
#include <sstream>
#include "token.h"

namespace nodetypes {
    using namespace std;

    const string Number = "NUMBER";
    const string BinaryOperator = "BINOP";
};

struct Node {
    std::string nodeType;
    Token token;
    std::string to_string() {
        if (nodeType == nodetypes::Number) {
            return token.value;
        } else if (nodeType == nodetypes::BinaryOperator) {
            return "(" + leftNode->to_string() + ", " + token.value + ", " + rightNode->to_string() + ")";
        } else {
            return "Not implemented! " + nodeType;
        }
    };

    Node(std::string nodeType, Token token) {
        std::ostringstream temp;
        temp << nodeType;
        this->nodeType = temp.str();
        this->token = token;
    }

    Node(std::string nodeType, std::shared_ptr<Node> leftNode, Token token, std::shared_ptr<Node> rightNode) {
        std::ostringstream temp;
        temp << nodeType;
        this->nodeType = temp.str();
        this->token = token;
        this->leftNode = leftNode;
        this->rightNode = rightNode;
    }


    std::shared_ptr<Node> leftNode;
    std::shared_ptr<Node> rightNode;
};

#endif // !AST_H