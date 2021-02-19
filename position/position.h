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

    Position(int index, int lineNumber, int columnNumber, std::string filename, std::string filetext) {
        this->index = index;
        this->lineNumber = lineNumber;
        this->columnNumber = columnNumber;
        this->filename = filename;
        this->filetext = filetext;
    }

    Position advance(char currentChar = ' ') {
        index++;
        columnNumber++;

        if (currentChar == '\n') {
            lineNumber++;
            columnNumber = 0;
        }

        return *this;
    }

    Position copy() {
        return Position(index, lineNumber, columnNumber, filename, filetext);
    }
};

#endif // !POSITION_H

