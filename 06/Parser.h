#ifndef ___PARSER_H___
#define ___PARSER_H___

#include <string>
#include <fstream>
#include <sstream>
#include "SymbolTable.h"

class Parser {
public:
    Parser(const std::string& inputAsmFile);
    void parse();

private:
    std::ifstream ifs;
    std::ofstream ofs;
    std::string currentInstruction;
    std::string filename;
    std::size_t instructionAddress;
    bool isCodeGenerationPhase;
    SymbolTable symboltable;
    enum INSTRUCTION_TYPE {
        A_INSTRUCTION,
        C_INSTRUCTION,
        L_INSTRUCTION 
    };

    bool hasMoreLines();
    void advance();
    void reset();
    const INSTRUCTION_TYPE instructionType();
    const std::string address();
    const std::string symbol();
    const std::string dest();
    const std::string comp();
    const std::string jump();
};

#endif