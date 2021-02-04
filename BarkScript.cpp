#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "lexer.h"

int main() {
    while (true) {
        //std::string input = "5+55";
        std::string input;
        std::getline(std::cin, input);
        Lexer lexer = Lexer(input);
        while (!lexer.finished) {
            Token token = lexer.nextToken();
            std::cout << token.to_string() << std::endl;
        }
        std::cout << std::endl;
    }
}
