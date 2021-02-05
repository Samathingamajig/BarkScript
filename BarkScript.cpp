#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "lexer.h"
#include "ast.h"
#include "parser.h"

const std::string bsversion = "0.0.3";

int main() {
    std::cout << "BarkScript version " << bsversion << std::endl;
    while (true) {
        //std::string input = "5+55";
        std::string input;
        std::cout << "bs > ";
        std::getline(std::cin, input);
        std::cout << std::endl;
        Lexer lexer = Lexer(input);
        std::vector<Token> tokenized = lexer.tokenize();
        for (Token token : tokenized) {
            std::cout << token.to_string() << std::endl;
        }
        std::cout << std::endl;
        Parser parser = Parser(tokenized);
        std::shared_ptr<Node> abSyTree = parser.expr();
        std::cout << "no error" << std::endl;
        std::cout << abSyTree->to_string() << std::endl;
        std::cout << "--------------------------" << std::endl;
    }
}
