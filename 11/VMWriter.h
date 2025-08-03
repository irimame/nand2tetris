#ifndef ___VMWRITER_H___
#define ___VMWRITER_H___

#include "TokenTypeEnum.h"
#include <string>
#include <fstream>

class VMWriter {
public:
    VMWriter(const std::string& inputJackFile);
    void writePush(const SEGMENT segment, const std::size_t index);
    void writePop(const SEGMENT segment, const std::size_t index);
    void writeArithmetic(const COMMAND command);
    void writeLabel(const std::string& label);
    void writeGoto(const std::string& label);
    void writeIf(const std::string& label);
    void writeCall(const std::string& name, std::size_t nArgs);
    void writeFunction(const std::string& name, std::size_t nVars);
    void writeReturn();
    void close();

private:
    std::ofstream ofs;
};

#endif