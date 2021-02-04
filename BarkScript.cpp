// BarkScript.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <iostream>
#include <string>
#include <vector>
#include <memory>

namespace tokens {
    using namespace std;
    string PLUS = "+";
    string INT = "INT";
    string EEOF = "EOF";
    string UNKNOWN = "UNKNOWN";
}

struct Token {
    std::string type;
    std::string value;
    int positionStart;
    int positionEnd;
    int line;

    Token(std::string type, std::string value, int positionStart, int positionEnd, int line) {
        this->type = type;
        this->value = value;
        this->positionStart = positionStart;
        this->positionEnd = positionEnd;
        this->line = line;
    }

    std::string to_string() {
        return "Token(" + type + ", \"" + value + "\", " + std::to_string(positionStart) + ", " + std::to_string(positionEnd) + ")";
    }
};

struct Lexer {
    std::string input;
    int position = 0;
    int nextPosition = 1;
    char current;
    int lineCount = 0;
    int inputLength;
    bool finished = false;

    Lexer(std::string input) {
        this->input = input;
        this->inputLength = input.length();
        if (inputLength > 0) {
            this->current = input[0];
        } else {
            this->current = 0;
        }
    }

    void readChar();
    char peekChar(int num);
    bool isNumeric(char c);
    Token nextToken();
};

void Lexer::readChar() {
    if (nextPosition >= inputLength) {
        current = 0;
    } else {
        current = input[nextPosition];
    }
    position = nextPosition;
    nextPosition++;
}

char Lexer::peekChar(int num = 0) {
    if (nextPosition + num >= inputLength) {
        return 0;
    } else {
        return input[nextPosition + num];
    }
}

Token Lexer::nextToken() {
    switch (current) {
    case '+': {
        Token token = Token(tokens::PLUS, "+", position, position, lineCount);
        readChar();
        return token;
    }
    case 0: {
        finished = true;
        return Token(tokens::EEOF, "", position, position, lineCount);
    }
    default: {
        if (isNumeric(current)) {
            std::string value = std::string(1, current);
            int start = position;
            int line = lineCount;
            readChar();
            while (isNumeric(current)) {
                value += current;
                readChar();
            }
            return Token(tokens::INT, value, start, position - 1, line);
        }
        char old = current;
        readChar();
        return Token(tokens::UNKNOWN, std::string(1, old), position - 1, position - 1, lineCount);
    }
    }
}

bool Lexer::isNumeric(char c) {
    return 48 <= c && c <= 57;
}

int main() {
    while (true) {
        //std::string input = "5+55";
        std::string input;
        std::cin >> input;
        Lexer lexer = Lexer(input);
        while (!lexer.finished) {
            Token token = lexer.nextToken();
            std::cout << token.to_string() << std::endl;
        }
        std::cout << std::endl;
    }
}









//#include <iostream>
//#include <string>
//#include <vector>
//#include <memory>
//
//struct Token {
//    std::string type;
//    virtual std::string to_string() { return "Token(" + type + ")"; };
//};
//
//struct IntToken : public Token {
//    IntToken(int value) {
//        this->value = value;
//        this->type = "INT";
//    }
//    int value;
//    std::string to_string() {
//        return "Token(INT, " + std::to_string(value) + ")";
//    }
//};
//
//struct PlusToken : public Token {
//    PlusToken() {
//        this->type = "PLUS";
//    }
//};
//
//std::vector<std::shared_ptr<Token>> tokenize(std::string input) {
//    std::vector<std::shared_ptr<Token>> tokens;
//    for (int i = 0; i < input.length(); i++) {
//        char c = input[i];
//        if (std::isdigit(c)) {
//            IntToken t = IntToken(c - 48);
//            int j = 0;
//            for (; j + i + 1< input.length(); j++) {
//                if (std::isdigit(input[i + j + 1])) {
//                    t.value *= 10;
//                    t.value += c - 48;
//                }
//                else break;
//            }
//			i += j;
//            
//            tokens.push_back(std::make_shared<IntToken>(t));
//        }
//        else if (c == '+') {
//            PlusToken p = PlusToken();
//            tokens.push_back(std::make_shared<PlusToken>(p));
//        }
//    }
//    return tokens;
//}
//
//template<typename Base, typename T>
//inline bool instanceof(const T* ptr) {
//    return dynamic_cast<const Base*>(ptr) != nullptr;
//}
//
//int main()
//{
//    //std::string input = "5+55";
//    while (true) {
//        std::string input;
//        std::cout << "Tokenize > ";
//        std::cin >> input;
//        std::vector<std::shared_ptr<Token>> tokens = tokenize(input);
//        for (int i = 0; i < tokens.size(); i++) {
//            //std::cout << tokens[i].to_string() << std::endl;
//            std::cout << tokens[i]->to_string() << std::endl;
//            if (instanceof<IntToken>(tokens[i].get())) {
//                // old was safely casted to NewType
//                IntToken* it = dynamic_cast<IntToken*>(tokens[i].get());
//                //std::cout << it->value << std::endl;
//            }
//        }
//        std::cout << std::endl;
//    }
//}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
