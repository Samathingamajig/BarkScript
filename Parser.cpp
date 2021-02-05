#include "parser.h"
#include <vector>
#include <memory>
#include <functional>
#include "token.h"
#include "tokens.h"
#include "ast.h"

// https://stackoverflow.com/a/20303915/12101554
bool in_array(const std::string& value, const std::vector<std::string>& array) {
    return std::find(array.begin(), array.end(), value) != array.end();
}

Parser::Parser(std::vector<Token> tokens) {
    this->tokens = tokens;
    this->tokenIndex = -1;
    nextToken();
}

Token Parser::nextToken() {
    tokenIndex++;
    if (tokenIndex < tokens.size()) {
        currentToken = tokens[tokenIndex];
    }
    return currentToken;
}

std::shared_ptr<Node> Parser::factor() {
    Token token = currentToken;
    if (token.type == tokens::NUMBER) {
        nextToken();
        Node temp = Node(nodetypes::Number, token);
        return std::make_shared<Node>(temp);
    }
}

std::shared_ptr<Node> Parser::term() {
    std::function<std::shared_ptr<Node>()> rule = [this]() { return factor(); };
    return binaryOperation(rule, { tokens::ASTERISK, tokens::F_SLASH });
}

std::shared_ptr<Node> Parser::expr() {
    std::function<std::shared_ptr<Node>()> rule = [this]() { return term(); };
    return binaryOperation(rule, { tokens::PLUS, tokens::MINUS });
}

std::shared_ptr<Node> Parser::binaryOperation(std::function<std::shared_ptr<Node>()> rule, std::vector<std::string> allowedTokens) {
    std::shared_ptr<Node> left = rule();

    while (in_array(currentToken.type, allowedTokens)) {
        Token operatorToken = currentToken;
        nextToken();
        std::shared_ptr<Node> right = rule();
        Node temp = Node(nodetypes::BinaryOperator, left, operatorToken, right);
        left = std::make_shared<Node>(temp);
    }

    return left;
}
