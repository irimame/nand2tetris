#include "SymbolTable.h"
#include <iostream>

SymbolTable::SymbolTable()
: staticCounter(0), fieldCounter(0), argCounter(0), varCounter(0)
{}

void SymbolTable::resetSubroutineSymbolTable() {
    subroutineSymbolTable.clear();
    argCounter = 0;
    varCounter = 0;
}

void SymbolTable::define(const std::string& name, const std::string& type, const VARIABLETYPE kind) {
    switch (kind) {
        case VARIABLETYPE::STATIC:
            classSymbolTable.push_back({
                name, type, VARIABLETYPE::STATIC, staticCounter++
            });
            break;
        case VARIABLETYPE::FIELD:
            classSymbolTable.push_back({
                name, type, VARIABLETYPE::FIELD, fieldCounter++
            });
            break;
        case VARIABLETYPE::ARG:
            subroutineSymbolTable.push_back({
                name, type, VARIABLETYPE::ARG, argCounter++
            });
            break;
        case VARIABLETYPE::VAR:
            subroutineSymbolTable.push_back({
                name, type, VARIABLETYPE::VAR, varCounter++
            });
            break;
        default:
            std::cerr
                << "Error: Unexpected variable type. " << std::endl;
            std::exit(EXIT_FAILURE);
    }
}

const std::size_t SymbolTable::varCount(const VARIABLETYPE kind) {
    switch (kind) {
        case VARIABLETYPE::STATIC:  return staticCounter;
        case VARIABLETYPE::FIELD:   return fieldCounter;
        case VARIABLETYPE::ARG:     return argCounter;
        case VARIABLETYPE::VAR:     return varCounter;
        default:
            std::cerr 
                << "Error: Variable type must be STATIC, FIELD, ARG or VAR. "
                << std::endl;
            std::exit(EXIT_FAILURE);
    }
}

const VARIABLETYPE SymbolTable::kindOf(const std::string& name) {
    for (auto e: subroutineSymbolTable) {
        if (std::get<0>(e) == name) {
            return std::get<2>(e);
        }
    }
    for (auto e: classSymbolTable) {
        if (std::get<0>(e) == name) {
            return std::get<2>(e);
        }
    }
    return VARIABLETYPE::NONE;
}

const std::string SymbolTable::typeOf(const std::string& name) {
    for (auto e: subroutineSymbolTable) {
        if (std::get<0>(e) == name) {
            return std::get<1>(e);
        }
    }
    for (auto e: classSymbolTable) {
        if (std::get<0>(e) == name) {
            return std::get<1>(e);
        }
    }
    std::cerr 
        << "Error: Variable '" << name << "' not found. "
        << std::endl;
    std::exit(EXIT_FAILURE);
}

const std::size_t SymbolTable::indexOf(const std::string& name) {
    for (auto e: subroutineSymbolTable) {
        if (std::get<0>(e) == name) {
            return std::get<3>(e);
        }
    }
    for (auto e: classSymbolTable) {
        if (std::get<0>(e) == name) {
            return std::get<3>(e);
        }
    }
    std::cerr 
        << "Error: Variable '" << name << "' not found. "
        << std::endl;
    std::exit(EXIT_FAILURE);
}

const SEGMENT SymbolTable::segmentOf(const std::string& varName) {
    VARIABLETYPE varType = kindOf(varName);
    switch (varType) {
        case VARIABLETYPE::STATIC:
            return SEGMENT::STATIC;
        case VARIABLETYPE::FIELD:
            return SEGMENT::THIS;
        case VARIABLETYPE::ARG:
            return SEGMENT::ARGUMENT;
        case VARIABLETYPE::VAR:
            return SEGMENT::LOCAL;
        default:
            std::cerr
                << "Error in segmentOf: Unknown variable '" 
                << varName << "'. " 
                << std::endl;
            std::exit(EXIT_FAILURE);
    }
}