#include "symboltable.h"
#include <unordered_map>
#include <string>
#include "../object/object.h"

std::unordered_map<std::string, spObject> globalConstantVariablesTable = {
    { "null", Null() },
    { "Infinity", Number("Infinity") },
    { "NaN", Number("NaN") },
    { "true", Boolean(true) },
    { "false", Boolean(false) },
};

bool isGlobalConstantVariable(const std::string& identifier) {
    return globalConstantVariablesTable.find(identifier) != globalConstantVariablesTable.end();
}

spObject SymbolTable::get(const std::string& key) const {
    if (isGlobalConstantVariable(key)) {
        return globalConstantVariablesTable.at(key);
    }
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

SymbolTableSetReturnCode SymbolTable::set(const std::string& key, const spObject& value, const bool forceCurrentContext) {
    // Returning `false` signifies that there was an error
    if (isGlobalConstantVariable(key)) {
        return SymbolTableSetReturnCode::errorGlobalConstantVariable;
    }
    if (forceCurrentContext) {
        symbols[key] = value;
        return SymbolTableSetReturnCode::perfect;
    } else {
        if (symbols.find(key) != symbols.end()) {
            symbols[key] = value;
            return SymbolTableSetReturnCode::perfect;
        } else if (parent != nullptr) {
            return parent->set(key, value, false);
        } else {
            // The symbol doesn't exist anywhere and we weren't told to make it
            return SymbolTableSetReturnCode::errorNotInScope;
        }
    }
}

bool SymbolTable::exists(const std::string& key, const bool deepSearch) const {
    if (symbols.find(key) != symbols.end()) {
        return true;
    } else if (deepSearch && parent != nullptr) {
        return parent->exists(key, deepSearch);
    } else {
        return false;
    }
}