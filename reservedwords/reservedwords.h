#pragma once
#ifndef RESERVEDWORDS_H
#define RESERVEDWORDS_H
#include <string>
#include <algorithm>

namespace reservedWords {
    using namespace std;

    const string LET = "let";
}

const std::string reservedWordsArray[] = {
    reservedWords::LET,
};

inline bool isReservedWord(std::string identifier) {
    return std::find(std::begin(reservedWordsArray), std::end(reservedWordsArray), identifier) != std::end(reservedWordsArray);
}

#endif // !RESERVEDWORDS_H
