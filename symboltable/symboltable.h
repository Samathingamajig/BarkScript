#pragma once
#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include <unordered_map>
#include <memory>
#include <string>

struct Object;

typedef std::shared_ptr<Object> spObject;

struct SymbolTable;

typedef std::shared_ptr<SymbolTable> spSymbolTable;

struct SymbolTable {
    std::unordered_map<std::string, spObject> symbols;
    spSymbolTable parent = nullptr;

    spObject get(std::string key);
    bool set(std::string key, spObject value, bool currentContext = false);
};


#endif // !SYMBOLTABLE_H
