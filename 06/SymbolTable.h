#ifndef ___SYMBOLTABLE_H___
#define ___SYMBOLTABLE_H___

#include <string>
#include <map>

class SymbolTable {
public:
    SymbolTable();
    void addEntry(const std::string &symbol, const std::size_t address);
    const bool contains(const std::string &symbol);
    const std::size_t getAddress(const std::string &symbol);
    const std::size_t newVariableAddress();

private:
    std::map<std::string, std::size_t> m;
    std::size_t variableAddressCounter;
};

#endif