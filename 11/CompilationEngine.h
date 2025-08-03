#ifndef ___COMPILATIONENGINE_H___
#define ___COMPILATIONENGINE_H___

#include <string>
#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"

class CompilationEngine {
public:
    CompilationEngine(const std::string& inputJackFile);
    void compileClass();
    void compileClassVarDec();
    void compileSubroutineDec();
    void compileParameterList();
    void compileSubroutineBody(const std::string& subroutineName, const std::string& subroutineType);
    void compileVarDec();
    void compileStatements();
    void compileLet();
    void compileIf();
    void compileWhile();
    void compileDo();
    void compileReturn();
    void compileExpression(bool isSubroutineCall = false);
    void compileTerm(bool isSubroutineCall = false);
    std::size_t compileExpressionList();
    void process(const std::string& str);

private:
    JackTokenizer tokenizer;
    SymbolTable symbolTable;
    VMWriter writer;
    std::ofstream ofs;
    std::string className;
    std::size_t labelIndex;
};

#endif