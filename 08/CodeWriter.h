#ifndef ___CODEWRITER_H___
#define ___CODEWRITER_H___

#include <fstream>
#include <string>
#include "CommandType.h"

class CodeWriter {
public:
    static void writeBootstrapCode(std::string directory);
    CodeWriter(const std::string& inputVMFile);
    void writeArithmetic(const std::string& command);
    void writePushPop(const CommandType::COMMAND_TYPE commandType, const std::string& segment, const std::size_t index);
    void writeLabel(const std::string& label);
    void writeGoto(const std::string& label);
    void writeIf(const std::string& label);
    void writeFunction(const std::string& functionName, const std::size_t nVars);
    void writeCall(const std::string& functionName, const std::size_t nArgs);
    void writeReturn();
    void setCurrentFunctionName(const std::string& functionName);
    void close();

private:
    static void writeCallSysinit(std::ofstream& ofs, const std::size_t nArgs);
    static std::string outputAsmFile;
    std::string VMFilename;
    std::ofstream ofs;
    std::size_t compCount;
    std::size_t returnAddCount;
    std::string currentFunctionName;
};

#endif