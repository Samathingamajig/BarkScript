#include <iostream>
#include <string>
#include <memory>
#include "token/token.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "object/object.h"
#include "interpreter/interpreter.h"
#include "context/context.h"

const std::string bsversion = "0.1.0";

int main() {
    std::cout << "BarkScript version " << bsversion << std::endl;
    spContext context = std::make_shared<Context>(Context("<main>"));
    context->symbolTable = std::make_shared<SymbolTable>(SymbolTable());
    while (true) {
        //std::string input = "5+55";
        std::string input;
        std::cout << "bs > ";
        std::getline(std::cin, input);
        std::cout << std::endl;
        if (std::cin.eof()) {
            return 0;
        }
        Lexer lexer = Lexer(input);
        MultiLexResult mlr = lexer.tokenize();
        if (mlr.hasError()) {
            std::cout << mlr.error->to_string() << std::endl;
            std::cout << "--------------------------" << std::endl;
            continue;
        }
        for (Token token : mlr.tokenized) {
            std::cout << token.to_string() << std::endl;
        }
        if (mlr.tokenized.size() != 1) {
            std::cout << std::endl;
            Parser parser = Parser(mlr.tokenized);
            ParseResult abSyTree = parser.parse();
            if (abSyTree.hasError()) {
                std::cout << abSyTree.error->to_string() << std::endl;
            } else {
                std::cout << abSyTree.node->to_string() << std::endl;
                std::cout << std::endl;

                Interpreter interpreter;
                RuntimeResult rt = interpreter.visit(abSyTree.node, context);
                if (rt.hasError()) {
                    std::cout << rt.error->to_string() << std::endl;
                } else {
                    std::cout << rt.object->to_string() << std::endl;
                }
            }
        }
        std::cout << "--------------------------" << std::endl;
    }
}
