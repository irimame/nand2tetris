#ifndef ___COMPILATIONENGINE_H___
#define ___COMPILATIONENGINE_H___

#include <string>
#include "JackTokenizer.h"

class CompilationEngine {
public:
    CompilationEngine(const std::string& inputJackFile);
    void compileClass();
    void compileClassVarDec();
    void compileSubroutineDec();
    void compileParameterList();
    void compileSubroutineBody();
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
    void printXMLToken();

private:
    JackTokenizer tokenizer;
    std::ofstream ofs;
};

#endif