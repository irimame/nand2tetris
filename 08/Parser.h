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
    std::string command, arg1;
    std::size_t arg2;

    bool hasMoreLines();
    void advance();
    CommandType::COMMAND_TYPE commandType();
    std::string getArg1();
    std::size_t getArg2();
};

#endif