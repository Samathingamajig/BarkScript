#include <iostream>
#include <string>
#include <memory>
#include "vendor/optionparser-1.7/optionparser.h"
#include "token/token.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "object/object.h"
#include "interpreter/interpreter.h"
#include "context/context.h"

const std::string bsversion = "0.1.8 alpha 2";

enum optionIndex { CLI_UNKNOWN, CLI_HELP, CLI_NODEBUG };
const option::Descriptor usage[] =
{
 {CLI_UNKNOWN, 0, "", "", option::Arg::None, "USAGE: example [options]\n\n"
                                        "Options:" },
 {CLI_HELP, 0, "h", "help", option::Arg::None, "  -h --help  \tPrint usage and exit." },
 {CLI_NODEBUG, 0, "nd", "nodebug", option::Arg::None, "  -nd --nodebug  \tDoes not print Lexer or Parser results." },
 {0,0,0,0,0,0}
};

int main(int argc, char* argv[]) {
    argc -= (argc > 0); argv += (argc > 0); // skip program name argv[0] if present
    option::Stats cli_stats(usage, argc, argv);
    std::vector<option::Option> cli_options(cli_stats.options_max);
    std::vector<option::Option> cli_buffer(cli_stats.buffer_max);
    option::Parser cli_parse(usage, argc, argv, &cli_options[0], &cli_buffer[0]);

    if (cli_parse.error())
        return 1;

    if (cli_options[CLI_HELP]) {
        option::printUsage(std::cout, usage);
        return 0;
    }

    bool printDebug = true;
    if (cli_options[CLI_NODEBUG]) {
        printDebug = false;
    }

    std::cout << "BarkScript version " << bsversion << std::endl;
    spContext context = std::make_shared<Context>(Context("<main>"));
    context->symbolTable = std::make_shared<SymbolTable>(SymbolTable());
    while (true) {
        //std::string input = "5+55";
        std::string input;
        std::cout << "bs > ";
        std::getline(std::cin, input);
        if (std::cin.eof()) {
            return 0;
        }
        Lexer lexer = Lexer(input);
        MultiLexResult mlr = lexer.tokenize();
        if (mlr.hasError()) {
            std::cout << std::endl;
            std::cout << mlr.error->to_string() << std::endl;
            std::cout << "--------------------------" << std::endl;
            continue;
        }
        if (printDebug) {
            std::cout << std::endl;
            for (Token token : mlr.tokenized) {
                std::cout << token.to_string() << std::endl;
            }
        }
        if (mlr.tokenized.size() != 1) {
            Parser parser = Parser(mlr.tokenized);
            ParseResult abSyTree = parser.parse();
            if (abSyTree.hasError()) {
                std::cout << std::endl;
                std::cout << abSyTree.error->to_string() << std::endl;
                std::cout << "--------------------------" << std::endl;
                continue;
            }
            if (printDebug) {
                std::cout << std::endl;
                std::cout << abSyTree.node->to_string() << std::endl;
                std::cout << std::endl;
            }

            Interpreter interpreter;
            RuntimeResult rt = interpreter.visit(abSyTree.node, context);
            if (rt.hasError()) {
                std::cout << rt.error->to_string() << std::endl;
                std::cout << "--------------------------" << std::endl;
                continue;
            }
            std::cout << rt.object->to_string() << std::endl;
        }
        if (printDebug) std::cout << "--------------------------" << std::endl;
    }
}
