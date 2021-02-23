#pragma once
#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include <unordered_map>
#include <memory>
#include <string>

enum class SymbolTableSetReturnCode {
    perfect,
    errorGlobalConstantVariable,
    errorUserDefinedConstantVariable,
    errorNotInScope,
};

struct Object;

typedef std::shared_ptr<Object> spObject;

struct SymbolTable;

typedef std::shared_ptr<SymbolTable> spSymbolTable;

extern std::unordered_map<std::string, spObject> globalConstantVariablesTable;

extern bool isGlobalConstantVariable(const std::string& identifier);

struct SymbolTable {
    std::unordered_map<std::string, spObject> symbols;
    spSymbolTable parent = nullptr;

    spObject get(const std::string& key) const;
    SymbolTableSetReturnCode set(const std::string& key, const spObject& value, const bool forceCurrentContext = false);

    bool exists(const std::string& key, const bool deepSearch = true) const;
};


#endif // !SYMBOLTABLE_H
