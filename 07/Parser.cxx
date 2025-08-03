#include "Parser.h"
#include <iostream>
#include <cctype>
#include <filesystem>
#include <cstdlib>

using ComType = CommandType::COMMAND_TYPE;

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

Parser::Parser(const std::string& inputVMFile)
: codewriter(inputVMFile) {
    std::filesystem::path p(inputVMFile);
    std::string VMFilename = p.stem();
    if (inputVMFile.substr(inputVMFile.size() - 3, 3) != ".vm") {
        std::cerr << "Error: Please specify .vm file. " << std::endl;
        std::exit(EXIT_FAILURE);
    }
    else if (!isupper(VMFilename.at(0))) {
        std::cerr << "Error: The first letter of input vm file must be upper case. " << std::endl;
        std::exit(EXIT_FAILURE);
    }

    ifs = std::ifstream(inputVMFile);
    if (!ifs) {
        std::cerr << "Error: Could not open the input vm file. " << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void Parser::parse() {
    while (true) {
        if (hasMoreLines()) advance();
        else break;
    }
    if (ifs.is_open()) ifs.close();
    codewriter.close();
}

bool Parser::hasMoreLines() {
    std::stringstream ss;
    std::string line, lineTrimmed;
    const std::string blank(" ");
    while (std::getline(ifs, line)) {
        lineTrimmed = line.substr(line.find_first_not_of(blank));
        if (isCommentLine(lineTrimmed) || isBlankLine(lineTrimmed)) continue;
        ss << lineTrimmed;
        ss >> op >> seg >> ind;
        return true;
    }
    return false;
}

void Parser::advance() {
    if (commandType() == ComType::C_ARITHMETIC) {
        codewriter.writeArithmetic(arg1());
    }
    else if (commandType() == ComType::C_PUSH || commandType() == ComType::C_POP) {
        codewriter.writePushPop(commandType(), arg1(), arg2());
    }
}

ComType Parser::commandType() {
    if (op == "add" || op == "sub" || op == "neg" || 
        op == "eq" || op == "gt" || op == "lt" || 
        op == "and" || op == "or" || op == "not") {
            return ComType::C_ARITHMETIC;
    }
    else if (op == "push") return ComType::C_PUSH;
    else if (op == "pop") return ComType::C_POP;
    else {
        std::cerr << "Error: Unknown commandType operation '" << op << "'. " << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

std::string Parser::arg1() {
    if (commandType() == ComType::C_ARITHMETIC) return op;
    else return seg;
}

std::size_t Parser::arg2() {
    return ind;
}