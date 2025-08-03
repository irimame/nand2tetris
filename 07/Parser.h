#ifndef ___PARSER_H___
#define ___PARSER_H___

#include <string>
#include <fstream>
#include "CommandType.h"
#include "CodeWriter.h"

class Parser {
public:
    Parser(const std::string& inputVMFile);
    void parse();

private:
    std::ifstream ifs;
    CodeWriter codewriter;
    std::string op, seg;
    std::size_t ind;

    bool hasMoreLines();
    void advance();
    CommandType::COMMAND_TYPE commandType();
    std::string arg1();
    std::size_t arg2();
};

#endif