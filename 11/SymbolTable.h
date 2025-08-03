#ifndef ___SYMBOLTABLE_H___
#define ___SYMBOLTABLE_H___

#include "TokenTypeEnum.h"
#include <string>
#include <vector>
#include <tuple>
#include <set>

class SymbolTable {
public:
    SymbolTable();
    void resetSubroutineSymbolTable();
    void define(const std::string& name, const std::string& type, const VARIABLETYPE kind);
    const std::size_t varCount(const VARIABLETYPE kind);
    const VARIABLETYPE kindOf(const std::string& name);
    const std::string typeOf(const std::string& name);
    const std::size_t indexOf(const std::string& name);
    const SEGMENT segmentOf(const std::string& varName);

private:
    std::vector<std::tuple<std::string, std::string, VARIABLETYPE, std::size_t>> classSymbolTable;
    std::vector<std::tuple<std::string, std::string, VARIABLETYPE, std::size_t>> subroutineSymbolTable;
    std::size_t staticCounter;
    std::size_t fieldCounter;
    std::size_t argCounter;
    std::size_t varCounter;
};

#endif