#include "parser.h"
#include <vector>
#include <memory>
#include <functional>
#include "../token/token.h"
#include "../token/tokens.h"
#include "../ast/ast.h"
#include "../reservedwords/reservedwords.h"

// https://stackoverflow.com/a/20303915/12101554
bool in_array(const std::string& value, const std::vector<std::string>& array) {
    return std::find(array.begin(), array.end(), value) != array.end();
}

Parser::Parser(const std::vector<Token>& tokens) {
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

Token Parser::peekToken(unsigned int num) const {
    if (tokenIndex + num + 1 >= tokens.size()) {
        return Token();
    } else {
        return tokens[tokenIndex + num + 1];
    }
}

ParseResult Parser::atom() {
    ParseResult pr;
    Token token = currentToken;
    if (token.type == tokens::NUMBER) {
        nextToken();
        pr.registerAdvancement();
        return pr.success(NumberNode(token));
    } else if (token.type == tokens::IDENTIFIER) {
        nextToken();
        pr.registerAdvancement();
        return pr.success(VariableRetrievementNode(token));
    } else if (token.type == tokens::OPEN_PAREN) {
        nextToken();
        pr.registerAdvancement();
        spNode value = pr.registerPR(expr());
        if (pr.hasError()) return pr;
        if (currentToken.type == tokens::CLOSE_PAREN) {
            nextToken();
            pr.registerAdvancement();
            return pr.success(value);
        }
        return pr.failure(InvalidSyntaxError(currentToken.positionStart, currentToken.positionEnd, "Expected a ')'"));
    } else {
        return pr.failure(InvalidSyntaxError(currentToken.positionStart, currentToken.positionEnd, "Expected a number, identifier, '+', '-', or a '('"));
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
        pr.registerAdvancement();
        spNode factorRes = pr.registerPR(factor());
        if (pr.hasError()) return pr;
        return pr.success(UnaryOperatorNode(token, factorRes));
    }
    return exponent();
}

ParseResult Parser::term() {
    std::function<ParseResult()> rule = [this]() { return factor(); };
    return binaryOperation(rule, { tokens::ASTERISK, tokens::F_SLASH, tokens::DOUBLE_F_SLASH });
}

ParseResult Parser::assignment() {
    ParseResult pr;
    if (!currentToken.matches(tokens::IDENTIFIER))
        return pr.failure(InvalidSyntaxError(currentToken.positionStart, currentToken.positionEnd, "Expected an identifier"));
    Token variableNameToken = currentToken;
    nextToken();
    pr.registerAdvancement();
    if (!currentToken.matches(tokens::EQUAL))
        return pr.failure(InvalidSyntaxError(currentToken.positionStart, currentToken.positionEnd, "Expected an '='"));
    nextToken();
    pr.registerAdvancement();
    spNode value = pr.registerPR(expr());
    if (pr.hasError()) return pr;
    return pr.success(VariableAssignmentNode(variableNameToken, value));
}

ParseResult Parser::declaration() {
    ParseResult pr;
    nextToken();
    pr.registerAdvancement();
    Token variableNameToken = currentToken;
    if (variableNameToken.type != tokens::IDENTIFIER)
        return pr.failure(InvalidSyntaxError(variableNameToken.positionStart, variableNameToken.positionEnd, "Expected an identifier"));
    spNode value = nullptr;
    nextToken();
    pr.registerAdvancement();
    if (currentToken.matches(tokens::EQUAL)) {
        nextToken();
        pr.registerAdvancement();
        value = pr.registerPR(expr());
    } else {
        value = VariableRetrievementNode(Token(tokens::IDENTIFIER, "null", variableNameToken.positionStart, variableNameToken.positionEnd, false));
    }
    if (pr.hasError())
        return pr;
    return pr.success(VariableDeclarationNode(variableNameToken, value));
}

ParseResult Parser::expr() {
    ParseResult pr;
    if (nextIsAssignment()) {
        return assignment();
    } else {
        std::function<ParseResult()> rule = [this]() { return term(); };
        spNode termRes = pr.registerPR(binaryOperation(rule, { tokens::PLUS, tokens::MINUS }));
        if (pr.hasError()) {
            return pr.failure(InvalidSyntaxError(currentToken.positionStart, currentToken.positionEnd, "Expected a number, identifier, '+', '-', or a '('"));
        }
        return pr.success(termRes);
    }
}

ParseResult Parser::statement() {
    if (currentToken.matches(tokens::KEYWORD, reservedWords::LET)) {
        return declaration();
    } else {
        ParseResult pr = expr();
        if (pr.hasError()) {
            return pr.failure(InvalidSyntaxError(currentToken.positionStart, currentToken.positionEnd, "Expected a 'let', number, identifier, '+', '-', or a '('"));
        }
        if (!pr.hasError() && currentToken.type != tokens::EEOF) {
            return pr.failure(InvalidSyntaxError(currentToken.positionStart, currentToken.positionEnd, "Expected a '+', '-', '*', '/', '**', '//', or a '('"));
        }
        return pr;
    }
}

ParseResult Parser::parse() {
    return statement();
}

ParseResult Parser::binaryOperation(const std::function<ParseResult()>& rule, const std::vector<std::string>& allowedTokens) {
    return binaryOperation(rule, allowedTokens, rule);
}

ParseResult Parser::binaryOperation(const std::function<ParseResult()>& rule1, const std::vector<std::string>& allowedTokens, const std::function<ParseResult()>& rule2) {
    ParseResult pr;
    spNode left = pr.registerPR(rule1());
    if (pr.hasError()) return pr;

    while (in_array(currentToken.type, allowedTokens)) {
        Token operatorToken = currentToken;
        nextToken();
        pr.registerAdvancement();
        spNode right = pr.registerPR(rule2());
        if (pr.hasError()) return pr;
        left = BinaryOperatorNode(left, operatorToken, right);
    }

    return pr.success(left);
}

bool Parser::nextIsAssignment() const {
    return peekToken(0).matches(tokens::EQUAL);
}
