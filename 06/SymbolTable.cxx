#include "SymbolTable.h"

SymbolTable::SymbolTable() : variableAddressCounter(16) {
    m.emplace("R0", 0);
    m.emplace("R1", 1);
    m.emplace("R2", 2);
    m.emplace("R3", 3);
    m.emplace("R4", 4);
    m.emplace("R5", 5);
    m.emplace("R6", 6);
    m.emplace("R7", 7);
    m.emplace("R8", 8);
    m.emplace("R9", 9);
    m.emplace("R10", 10);
    m.emplace("R11", 11);
    m.emplace("R12", 12);
    m.emplace("R13", 13);
    m.emplace("R14", 14);
    m.emplace("R15", 15);
    m.emplace("SP", 0);
    m.emplace("LCL", 1);
    m.emplace("ARG", 2);
    m.emplace("THIS", 3);
    m.emplace("THAT", 4);
    m.emplace("SCREEN", 16384);
    m.emplace("KBD", 24576);
}

void SymbolTable::addEntry(const std::string &symbol, const std::size_t address) {
    m.emplace(symbol, address);
}

const bool SymbolTable::contains(const std::string &symbol) {
    if(m.find(symbol) != m.end()) return true;
    else return false;
}

const std::size_t SymbolTable::getAddress(const std::string &symbol) {
    return m.at(symbol);
}


const std::size_t SymbolTable::newVariableAddress() {
    return this->variableAddressCounter++;
}