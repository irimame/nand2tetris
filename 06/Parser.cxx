#include "Parser.h"
#include "Code.h"
#include "SymbolTable.h"
#include <iostream>
#include <cstdlib>
#include <bitset>
#include <algorithm>
#include <cctype>

namespace {
    bool isCommentLine(std::string& line) {
        if (line.size() < 2) return false;
        else if (line.substr(0, 2) == "//") return true;
        else return false;
    }

    bool isBlankLine(std::string& line) {
        if (line.at(0) == '\r') return true;
        else return false;
    }
}

Parser::Parser(const std::string& inputAsmFile) 
: instructionAddress(0), isCodeGenerationPhase(false) 
{
    if (inputAsmFile.substr(inputAsmFile.size() - 4, 4) != ".asm") {
        std::cerr << "Error: Please specify .asm file. " << std::endl;
        std::exit(EXIT_FAILURE);
    }
    ifs = std::ifstream(inputAsmFile);
    if (!ifs) {
        std::cerr << "Error: Could not open the input asm file. " << std::endl;
        std::exit(EXIT_FAILURE);
    }
    filename = inputAsmFile.substr(0, inputAsmFile.size() - 4);
    ofs = std::ofstream(filename + ".hack");
    if (!ofs) {
        std::cerr << "Error: Could not open the output hack file. " << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void Parser::parse() {
    while (true) {
        if (hasMoreLines()) advance();
        else break;
    }
    reset();
    while (true) {
        if (hasMoreLines()) advance();
        else break;
    }
}

bool Parser::hasMoreLines() {
    std::stringstream ss;
    std::string line, lineTrimmed;
    const std::string blank(" ");
    while (std::getline(ifs, line)) {
        lineTrimmed = line.substr(line.find_first_not_of(blank));
        if (isCommentLine(lineTrimmed) || isBlankLine(lineTrimmed)) continue;
        ss << lineTrimmed;
        ss >> currentInstruction;
        return true;
    }
    if (this->isCodeGenerationPhase && ifs.is_open()) ifs.close();
    if (this->isCodeGenerationPhase && ofs.is_open()) ofs.close();
    return false;
}

void Parser::advance() {
    if (this->isCodeGenerationPhase) {
        if (instructionType() == A_INSTRUCTION) {
            ofs << '0' << address() << std::endl;
        }
        else if (instructionType() == C_INSTRUCTION) {
            ofs << "111"
                << Code::comp(comp())
                << Code::dest(dest())
                << Code::jump(jump())
                << std::endl;
        }
    }
    else {
        if (instructionType() == L_INSTRUCTION) {
            symboltable.addEntry(symbol(), this->instructionAddress);
        }
        else ++(this->instructionAddress);
    }

}

void Parser::reset() {
    ifs.clear();
    ifs.seekg(0, std::ios::beg);
    this->isCodeGenerationPhase = true;
}

const Parser::INSTRUCTION_TYPE Parser::instructionType() {
    if (currentInstruction.at(0) == '@') return A_INSTRUCTION;
    else if (currentInstruction.at(0) == '(') return L_INSTRUCTION;
    else return C_INSTRUCTION;
}

const std::string Parser::address() {
    std::string currentSymbol = symbol();

    if (isdigit(currentSymbol.at(0))) {
        return std::bitset<15>(std::stoul(currentSymbol)).to_string();
    }
    else {
        if (!symboltable.contains(currentSymbol)) {
            symboltable.addEntry(currentSymbol, symboltable.newVariableAddress());
        }
        return std::bitset<15>(symboltable.getAddress(currentSymbol)).to_string();
    }
}

const std::string Parser::symbol() {
    if (currentInstruction.at(0) == '@') return currentInstruction.substr(1);
    else return currentInstruction.substr(1, currentInstruction.size() - 2);
}

const std::string Parser::dest() {
    std::size_t posEqual = currentInstruction.find('=');
    if (posEqual == std::string::npos) return "null";
    else return currentInstruction.substr(0, posEqual);
}

const std::string Parser::comp() {
    std::size_t l = 0;
    std::size_t r = currentInstruction.size();
    std::size_t posEqual = currentInstruction.find('=');
    std::size_t posSemicolon = currentInstruction.find(';');
    if (posEqual != std::string::npos) l = posEqual + 1;
    if (posSemicolon != std::string::npos) r = posSemicolon;
    return currentInstruction.substr(l, r - l);
}

const std::string Parser::jump() {
    std::size_t posSemicolon = currentInstruction.find(';');
    if (posSemicolon == std::string::npos) return "null";
    else return currentInstruction.substr(posSemicolon + 1);
}