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
        return pr.success(makeSharedNode(NumberNode(token)));
    } else if (in_array(token.type, { tokens::PLUS, tokens::MINUS })) {
        nextToken();
        spNode factorRes = pr.registerPR(factor());
        if (pr.hasError()) return pr;
        return pr.success(makeSharedNode(UnaryOperatorNode(token, factorRes)));
    } else if (token.type == tokens::OPEN_PAREN) {
        nextToken();
        spNode exprRes = pr.registerPR(expr());
        if (pr.hasError()) return pr;
        if (currentToken.type == tokens::CLOSE_PAREN) {
            nextToken();
            return pr.success(exprRes);
        }
        return pr.failure(makeSharedError(InvalidSyntaxError(currentToken.positionStart, currentToken.positionEnd, "Expected a ')'")));
    } else {
        pr.registerPR(makeSharedNode(ErrorNode(token)));
        return pr.failure(makeSharedError(InvalidSyntaxError(token.positionStart, token.positionEnd, "Expected a number")));
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
    if (currentToken.type == tokens::EEOF) {
        ParseResult pr;
        return pr.success(makeSharedNode(Node()));
    }
    ParseResult pr = expr();
    if (!pr.hasError() && currentToken.type != tokens::EEOF) {
        return pr.failure(makeSharedError(InvalidSyntaxError(currentToken.positionStart, currentToken.positionEnd, "Expected +, -, *, /")));
    }
    return pr;
}

ParseResult Parser::binaryOperation(std::function<ParseResult()> rule, std::vector<std::string> allowedTokens) {
    ParseResult pr;
    spNode left = pr.registerPR(rule());
    if (pr.hasError()) return pr;

    while (in_array(currentToken.type, allowedTokens)) {
        Token operatorToken = currentToken;
        nextToken();
        spNode right = pr.registerPR(rule());
        if (pr.hasError()) return pr;
        left = makeSharedNode(BinaryOperatorNode(left, operatorToken, right));
    }

    return pr.success(left);
}
