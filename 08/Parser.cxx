#include "Parser.h"
#include <iostream>
#include <cctype>
#include <filesystem>
#include <cstdlib>
#include <vector>

using ComType = CommandType::COMMAND_TYPE;

namespace {
    bool isCommentLine(std::string& line) {
        if (line.size() < 2) return false;
        else if (line.substr(0, 2) == "//") return true;
        else return false;
    }

    bool isBlankLine(std::string& line) {
        if (line.empty() || line.at(0) == '\r') return true;
        else return false;
    }
}

Parser::Parser(const std::string& inputVMFile)
: codewriter(inputVMFile) {
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
    const std::string whiteSpaceOrTab(" \t");

    while (std::getline(ifs, line)) {
        if (isBlankLine(line)) continue;
        lineTrimmed = line.substr(line.find_first_not_of(whiteSpaceOrTab));
        if (isCommentLine(lineTrimmed)) continue;
        ss << lineTrimmed;
        ss >> command >> arg1 >> arg2;
        return true;
    }
    return false;
}

void Parser::advance() {
    if (commandType() == ComType::C_ARITHMETIC) {
        codewriter.writeArithmetic(getArg1());
    }
    else if (commandType() == ComType::C_PUSH || commandType() == ComType::C_POP) {
        codewriter.writePushPop(commandType(), getArg1(), getArg2());
    }
    else if (commandType() == ComType::C_LABEL) {
        codewriter.writeLabel(getArg1());
    }
    else if (commandType() == ComType::C_GOTO) {
        codewriter.writeGoto(getArg1());
    }
    else if (commandType() == ComType::C_IF) {
        codewriter.writeIf(getArg1());
    }
    else if (commandType() == ComType::C_FUNCTION) {
        codewriter.setCurrentFunctionName(getArg1());
        codewriter.writeFunction(getArg1(), getArg2());
    }
    else if (commandType() == ComType::C_CALL) {
        codewriter.writeCall(getArg1(), getArg2());
    }
    else {
        codewriter.writeReturn();
    }
}

ComType Parser::commandType() {
    if (command == "add" || command == "sub" || command == "neg" || 
        command == "eq" || command == "gt" || command == "lt" || 
        command == "and" || command == "or" || command == "not") {
            return ComType::C_ARITHMETIC;
    }
    else if (command == "push") return ComType::C_PUSH;
    else if (command == "pop") return ComType::C_POP;
    else if (command == "label") return ComType::C_LABEL;
    else if (command == "goto") return ComType::C_GOTO;
    else if (command == "if-goto") return ComType::C_IF;
    else if (command == "function") return ComType::C_FUNCTION;
    else if (command == "return") return ComType::C_RETURN;
    else if (command == "call") return ComType::C_CALL;
    else {
        std::cerr << "Error: Unknown commandType operation '" << command << "'. " << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

std::string Parser::getArg1() {
    if (commandType() == ComType::C_ARITHMETIC) return command;
    else return arg1;
}

std::size_t Parser::getArg2() {
    return arg2;
}