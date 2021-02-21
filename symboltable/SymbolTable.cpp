#include "symboltable.h"
#include <unordered_map>
#include <string>
#include "../object/object.h"

std::unordered_map<std::string, spObject> globalConstantVariablesTable = {
    { "null", makeSharedObject(Null()) },
    { "Infinity", makeSharedObject(Number("Infinity")) },
    { "NaN", makeSharedObject(Number("NaN")) },
    { "true", makeSharedObject(Boolean(true)) },
    { "false", makeSharedObject(Boolean(false)) },
};

bool isGlobalConstantVariable(std::string identifier) {
    return globalConstantVariablesTable.find(identifier) != globalConstantVariablesTable.end();
}

spObject SymbolTable::get(std::string key) {
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

SymbolTableSetReturnCode SymbolTable::set(std::string key, spObject value, bool currentContext) {
    // Returning `false` signifies that there was an error
    if (isGlobalConstantVariable(key)) {
        return SymbolTableSetReturnCode::errorGlobalConstantVariable;
    }
    if (currentContext) {
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
