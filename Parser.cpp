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
    if ((unsigned) tokenIndex < tokens.size()) {
        currentToken = tokens[tokenIndex];
    }
    return currentToken;
}

ParseResult Parser::atom() {
    ParseResult pr;
    Token token = currentToken;
    if (token.type == tokens::NUMBER) {
        nextToken();
        return pr.success(makeSharedNode(NumberNode(token)));
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
        return pr.failure(makeSharedError(InvalidSyntaxError(currentToken.positionStart, currentToken.positionEnd, "Expected a number, '+', '-', or '('")));
    }
}

ParseResult Parser::exponent() {
    std::function<ParseResult()> rule1 = [this]() { return atom(); };
    std::function<ParseResult()> rule2 = [this]() { return factor(); };
    return binaryOperation(rule1, { tokens::DOUBLE_ASTERISK }, rule2);
}

ParseResult Parser::factor() {
    ParseResult pr;
    Token token = currentToken;
    if (in_array(token.type, { tokens::PLUS, tokens::MINUS })) {
        nextToken();
        spNode factorRes = pr.registerPR(factor());
        if (pr.hasError()) return pr;
        return pr.success(makeSharedNode(UnaryOperatorNode(token, factorRes)));
    }
    return exponent();
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
        return pr.failure(makeSharedError(InvalidSyntaxError(currentToken.positionStart, currentToken.positionEnd, "Expected a '+', '-', '*', '/', '**', '('")));
    }
    return pr;
}

ParseResult Parser::binaryOperation(std::function<ParseResult()> rule, std::vector<std::string> allowedTokens) {
    return binaryOperation(rule, allowedTokens, rule);
}

ParseResult Parser::binaryOperation(std::function<ParseResult()> rule1, std::vector<std::string> allowedTokens, std::function<ParseResult()> rule2) {
    ParseResult pr;
    spNode left = pr.registerPR(rule1());
    if (pr.hasError()) return pr;

    while (in_array(currentToken.type, allowedTokens)) {
        Token operatorToken = currentToken;
        nextToken();
        spNode right = pr.registerPR(rule2());
        if (pr.hasError()) return pr;
        left = makeSharedNode(BinaryOperatorNode(left, operatorToken, right));
    }

    return pr.success(left);
}

//ParseResult Parser::binaryOperationRightToLeft(std::function<ParseResult()> rule, std::vector<std::string> allowedTokens) {
//    ParseResult pr;
//    spNode left = pr.registerPR(rule());
//    if (pr.hasError()) return pr;
//    std::vector<Token> tokenStack;
//    std::vector<spNode> prStack = { left };
//
//    while (in_array(currentToken.type, allowedTokens)) {
//        tokenStack.push_back(currentToken);
//        nextToken();
//        spNode next = pr.registerPR(rule());
//        if (pr.hasError()) return pr;
//        prStack.push_back(next);
//    }
//
//    spNode result = prStack.back();
//    for (int i = prStack.size() - 2; i >= 0; i++) {
//        result = makeSharedNode(BinaryOperatorNode(prStack[i], tokenStack[i], result));
//    }
//
//    return pr.success(result);
//}
