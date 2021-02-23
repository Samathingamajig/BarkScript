#pragma once
#ifndef POSITION_H
#define POSITION_H
#include <string>

struct Position {
    int index;
    int lineNumber;
    int columnNumber;
    std::string filename;
    std::string filetext;

    Position() {
        index = 0;
        lineNumber = 0;
        columnNumber = 0;
        filename = "UNKNOWN_FILE";
        filetext = "UNKNOWN_FILE_TEXT";
    }

    Position(const int& index, const int& lineNumber, const int& columnNumber, const std::string& filename, const std::string& filetext) {
        this->index = index;
        this->lineNumber = lineNumber;
        this->columnNumber = columnNumber;
        this->filename = filename;
        this->filetext = filetext;
    }

    Position advance(const char& currentChar = ' ') {
        index++;
        columnNumber++;

        if (currentChar == '\n') {
            lineNumber++;
            columnNumber = 0;
        }

        return *this;
    }
};

#endif // !POSITION_H
