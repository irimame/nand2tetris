#ifndef ___CODEWRITER_H___
#define ___CODEWRITER_H___

#include <fstream>
#include <string>
#include "CommandType.h"

class CodeWriter {
public:
    CodeWriter(const std::string& inputVMFile);
    void writeArithmetic(const std::string& command);
    void writePushPop(const CommandType::COMMAND_TYPE commandType, const std::string& segment, const std::size_t index);
    void close();

private:
    std::string VMFilename;
    std::ofstream ofs;
    std::size_t compCount;
    void popxy();
    void popy();
    void push();
    void pop();
};

#endif