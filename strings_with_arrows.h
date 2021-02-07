#pragma once
#ifndef STRINGS_WITH_ARROWS_H
#define STRINGS_WITH_ARROWS_H
#include <string>
#include <regex>
#include "position.h"

// Translation from python: https://github.com/davidcallanan/py-myopl-code/blob/master/ep2/strings_with_arrows.py
inline std::string strings_with_arrows(std::string text, Position positionStart, Position positionEnd) {
    std::string result = "";

    // Calculate indices
    int indexStart = text.substr(0, positionStart.index).rfind('\n');
    if (indexStart < 0) indexStart = 0;
    int indexEnd = text.substr(indexStart + 1).find('\n');
    if (indexEnd < 0) indexEnd = text.size(); else indexEnd += indexEnd + 1;

    // Find the number of lines
    int lineCount = positionEnd.lineNumber - positionStart.lineNumber + 1;

    for (int i = 0; i < lineCount; i++) {
        // Calculate line columns
        std::string line = text.substr(indexStart, indexEnd);
        int columnStart = 0;
        if (i == 0) columnStart = positionStart.columnNumber;
        int columnEnd = line.length() - 1;
        if (i == lineCount - 1) columnEnd = positionEnd.columnNumber;

        // Append the result
        result += line + "\n";
        result += std::string(columnStart, ' ') + std::string(columnEnd - columnStart, '^');

        // Recalculate the indices
        indexStart = indexEnd;
        try {
            indexEnd = text.substr(indexStart + 1).find('\n');
        } catch (...) {
            indexEnd = -1;
        }
        if (indexEnd < 0) indexEnd = text.size(); else indexEnd += indexEnd + 1;
    }

    // Get rid of tabs (I don't know why this is done, but the original does this, so I'll do it too)
    // Adding regex here, just to do this, is probably really inefficient
    return std::regex_replace(result, std::regex("\t"), "");
}

#endif // !STRINGS_WITH_ARROWS_H

