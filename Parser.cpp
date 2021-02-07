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

ParseResult Parser::factor() {
    ParseResult pr;
    Token token = currentToken;
    if (token.type == tokens::NUMBER) {
        nextToken();
        return pr.success(std::make_shared<Node>(Node(nodetypes::Number, token)));
    } else {
        pr.registerPR(std::make_shared<Node>(Node(nodetypes::Error, token)));
        return pr.failure(Error(token.positionStart, token.positionEnd, errortypes::InvalidSyntaxError, "Expected a number"));
    }
}

ParseResult Parser::term() {
    std::function<ParseResult()> rule = [this]() { return factor(); };
    return binaryOperation(rule, { tokens::ASTERISK, tokens::F_SLASH });
}

ParseResult Parser::expr() {
    std::function<ParseResult()> rule = [this]() { return term(); };
    return binaryOperation(rule, { tokens::PLUS, tokens::MINUS });
}

ParseResult Parser::parse() {
    ParseResult pr = expr();
    if (!pr.hasError() && currentToken.type != tokens::EEOF) {
        return pr.failure(Error(currentToken.positionStart, currentToken.positionEnd, errortypes::InvalidSyntaxError, "Expected +, -, *, /"));
    }
    return pr;
}

ParseResult Parser::binaryOperation(std::function<ParseResult()> rule, std::vector<std::string> allowedTokens) {
    ParseResult pr;
    std::shared_ptr<Node> left = pr.registerPR(rule());
    if (pr.hasError()) return pr;

    while (in_array(currentToken.type, allowedTokens)) {
        Token operatorToken = currentToken;
        nextToken();
        std::shared_ptr<Node> right = pr.registerPR(rule());
        if (pr.hasError()) return pr;
        Node temp = Node(nodetypes::BinaryOperator, left, operatorToken, right);
        left = std::make_shared<Node>(temp);
    }

    return pr.success(left);
}
