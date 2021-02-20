#include "symboltable.h"
#include <unordered_map>
#include <string>
#include "../object/object.h"


spObject SymbolTable::get(std::string key) {
    if (symbols.find(key) == symbols.end()) {
        // Key not found
        if (parent != nullptr) {
            return parent->get(key);
        } else {
            // throw error
            return nullptr;
        }
    } else {
        return symbols.at(key);
    }
}

bool SymbolTable::set(std::string key, spObject value, bool currentContext) {
    // Returning `false` signifies that there was an error
    if (currentContext) {
        symbols[key] = value;
        return true;
    } else {
        if (symbols.find(key) != symbols.end()) {
            symbols[key] = value;
            return true;
        } else if (parent != nullptr) {
            return parent->set(key, value, false);
        } else {
            // The symbol doesn't exist anywhere and we weren't told to make it
            return false;
        }
    }
}
