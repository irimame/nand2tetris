#include "CompilationEngine.h"
#include <iostream>

CompilationEngine::CompilationEngine(const std::string& inputJackFile)
: tokenizer(inputJackFile), writer(inputJackFile), labelIndex(0)
{
    tokenizer.tokenize();
}

/**
 * class: 'class' className '{' classVarDec* subroutineDec* '}'
 */
void CompilationEngine::compileClass() {
    process("class");
    className = tokenizer.advance(); // className
    process("{");
    while (tokenizer.peek() == "static" || tokenizer.peek() == "field") {
        compileClassVarDec();
    }
    while (tokenizer.peek() == "constructor" || tokenizer.peek() == "function" || tokenizer.peek() == "method") {
        compileSubroutineDec();
    }
    process("}");
}

/**
 * classVarDec: ('static' | 'field') type varName (',' varName)* ';'
 */
void CompilationEngine::compileClassVarDec() {
    std::string kind, dataType, varName;
    VARIABLETYPE varType;
    kind = tokenizer.advance(); // 'static' or 'field'
    dataType = tokenizer.advance(); // type
    varName = tokenizer.advance(); // varName
    if (kind == "static") varType = VARIABLETYPE::STATIC;
    else varType = VARIABLETYPE::FIELD;
    symbolTable.define(varName, dataType, varType);
    while (tokenizer.peek() == ",") {
        process(",");
        varName = tokenizer.advance(); // varName
        symbolTable.define(varName, dataType, varType);
    }
    process(";");
}

/**
 * subroutineDec: ('constructor' | 'function' | 'method' ) ('void' | type) subroutineName
 *                  '(' parameterList ')' subroutineBody
 */
void CompilationEngine::compileSubroutineDec() {
    std::string subroutineType, dataType, subroutineName;
    symbolTable.resetSubroutineSymbolTable();
    subroutineType = tokenizer.advance(); // 'constructor' or 'function' or 'method'
    dataType = tokenizer.advance(); // 'void' or type
    subroutineName = tokenizer.advance(); // subroutineName
    if (subroutineType == "method") symbolTable.define("this", className, VARIABLETYPE::ARG);
    process("(");
    compileParameterList();
    process(")");
    compileSubroutineBody(subroutineName, subroutineType);
}

/**
 * parameterList: ((type varName) (',' type varName)*)?
 */
void CompilationEngine::compileParameterList() {
    std::string dataType, varName;
    if (tokenizer.peek() == "int" || tokenizer.peek() == "char" || tokenizer.peek() == "boolean" || 
        tokenizer.peekType() == TOKENTYPES::IDENTIFIER) 
    {
        dataType = tokenizer.advance(); // type
        varName = tokenizer.advance(); // varName
        symbolTable.define(varName, dataType, VARIABLETYPE::ARG);
        while (tokenizer.peek() == ",") {
            process(",");
            dataType = tokenizer.advance(); // type
            varName = tokenizer.advance(); // varName
            symbolTable.define(varName, dataType, VARIABLETYPE::ARG);
        }
    }
}

/**
 * subroutineBody: '{' varDec* statements '}'
 */
void CompilationEngine::compileSubroutineBody(const std::string& subroutineName, const std::string& subroutineType) {
    process("{");
    while (tokenizer.peek() == "var") {
        compileVarDec();
    }
    writer.writeFunction(className + "." + subroutineName, symbolTable.varCount(VARIABLETYPE::VAR));
    if (subroutineType == "constructor") {
        writer.writePush(SEGMENT::CONSTANT, symbolTable.varCount(VARIABLETYPE::FIELD));
        writer.writeCall("Memory.alloc", 1);
        writer.writePop(SEGMENT::POINTER, 0);
    }
    else if (subroutineType == "method") {
        writer.writePush(SEGMENT::ARGUMENT, 0);
        writer.writePop(SEGMENT::POINTER, 0);
    }
    compileStatements();
    process("}");
}

/**
 * varDec: 'var' type varName (',' varName)* ';'
 */
void CompilationEngine::compileVarDec() {
    std::string dataType, varName;
    process("var");
    dataType = tokenizer.advance(); // type
    varName = tokenizer.advance(); // varName
    symbolTable.define(varName, dataType, VARIABLETYPE::VAR);
    while (tokenizer.peek() == ",") {
        process(",");
        varName = tokenizer.advance(); // varName
        symbolTable.define(varName, dataType, VARIABLETYPE::VAR);
    }
    process(";");
}

/**
 * statements: statement*
 * statement: letStatement | ifStatement | whileStatement | doStatement | returnStatement
 */
void CompilationEngine::compileStatements() {
    while (tokenizer.peek() == "let" || tokenizer.peek() == "if" || tokenizer.peek() == "while" ||
            tokenizer.peek() == "do" || tokenizer.peek() == "return")
    {
        if (tokenizer.peek() == "let") compileLet();
        else if (tokenizer.peek() == "if") compileIf();
        else if (tokenizer.peek() == "while") compileWhile();
        else if (tokenizer.peek() == "do") compileDo();
        else if (tokenizer.peek() == "return") compileReturn();
    }
}

/**
 * letStatement: 'let' varName ('[' expression ']')? '=' expression ';'
 */
void CompilationEngine::compileLet() {
    process("let");
    std::string varName = tokenizer.advance(); // varName
    bool isArray = (tokenizer.peek() == "[");
    if (isArray) {
        // push @(varName[expression1]);
        // push varName;
        writer.writePush(symbolTable.segmentOf(varName), symbolTable.indexOf(varName));
        // push expression1; add;
        process("[");
        compileExpression();
        process("]");
        writer.writeArithmetic(COMMAND::ADD);
    }
    process("=");
    // pop expression2 to temp 0; 
    compileExpression();
    if (isArray) {
        // let varName[expression1] = expression2;
        writer.writePop(SEGMENT::TEMP, 0);
        writer.writePop(SEGMENT::POINTER, 1);
        writer.writePush(SEGMENT::TEMP, 0);
        writer.writePop(SEGMENT::THAT, 0);
    }
    else {
        // let varName = expression2;
        writer.writePop(symbolTable.segmentOf(varName), symbolTable.indexOf(varName));
    }
    process(";");
}

/**
 * ifStatement: 'if' '(' expression ')' '{' statements '}' ('else' '{' statements '}')?
 */
void CompilationEngine::compileIf() {
    std::string trueLabel = "LABELT" + std::to_string(labelIndex);
    std::string falseLabel = "LABELF" + std::to_string(labelIndex);
    ++labelIndex;
    process("if");
    process("(");
    compileExpression();
    process(")");
    // Invert expression (boolean) value
    writer.writeArithmetic(COMMAND::NOT);
    // if-goto false (The original expression is true)
    writer.writeIf(falseLabel);
    process("{");
    compileStatements();
    process("}");
    writer.writeGoto(trueLabel);
    // if-goto true (The original expression is false)
    writer.writeLabel(falseLabel);
    // else (optional)
    if (tokenizer.peek() == "else") {
        process("else");
        process("{");
        compileStatements();
        process("}");
    }
    // if-goto end
    writer.writeLabel(trueLabel);
}

/**
 * whileStatement: 'while' '(' expression ')' '{' statements '}'
 */
void CompilationEngine::compileWhile() {
    std::string whileLoopLabel = "LABELW" + std::to_string(labelIndex);
    std::string endLabel = "LABELE" + std::to_string(labelIndex);
    ++labelIndex;
    process("while");
    writer.writeLabel(whileLoopLabel);
    process("(");
    compileExpression();
    process(")");
    // Invert expression (boolean) value
    writer.writeArithmetic(COMMAND::NOT);
    // if-goto false (The original expression is true); jump to the end of while loop
    writer.writeIf(endLabel);
    // if-goto true (The original expression is false)
    process("{");
    compileStatements();
    process("}");
    writer.writeGoto(whileLoopLabel);
    // The end of while loop
    writer.writeLabel(endLabel);
}

/**
 * doStatement: 'do' subroutineCall ';'
 *              -> 'do' expression ';'
 */
void CompilationEngine::compileDo() {
    process("do");
    compileExpression(true);
    writer.writePop(SEGMENT::TEMP, 0);
    process(";");
}

/**
 * returnStatement: 'return' expression? ';'
 */
void CompilationEngine::compileReturn() {
    process("return");
    if (
        tokenizer.peekType() == TOKENTYPES::INT_CONST ||
        tokenizer.peekType() == TOKENTYPES::STRING_CONST ||
        tokenizer.peekType() == TOKENTYPES::KEYWORD ||
        tokenizer.peekType() == TOKENTYPES::IDENTIFIER ||
        tokenizer.peek() == "(" || tokenizer.peek() == "-" ||
        tokenizer.peek() == "~"
    ) {
        compileExpression();
    }
    // void function/method
    else {
        writer.writePush(SEGMENT::CONSTANT, 0);
    }
    process(";");
    writer.writeReturn();
}

/**
 * expression: term (op term)*
 */
void CompilationEngine::compileExpression(bool isSubroutineCall) {
    std::string op;
    compileTerm(isSubroutineCall);
    while (tokenizer.peek() == "+" || tokenizer.peek() == "-" || tokenizer.peek() == "*" ||
            tokenizer.peek() == "/" || tokenizer.peek() == "&" || tokenizer.peek() == "|" ||
            tokenizer.peek() == "<" || tokenizer.peek() == ">" || tokenizer.peek() == "=")
    {
        op = tokenizer.advance(); // op
        compileTerm(isSubroutineCall);
        // term1 op term2 -> push term1; push term2; op;
        if (op == "+") writer.writeArithmetic(COMMAND::ADD);
        else if (op == "-") writer.writeArithmetic(COMMAND::SUB);
        else if (op == "*") writer.writeCall("Math.multiply", 2);
        else if (op == "/") writer.writeCall("Math.divide", 2);
        else if (op == "&") writer.writeArithmetic(COMMAND::AND);
        else if (op == "|") writer.writeArithmetic(COMMAND::OR);
        else if (op == "<") writer.writeArithmetic(COMMAND::LT);
        else if (op == ">") writer.writeArithmetic(COMMAND::GT);
        else if (op == "=") writer.writeArithmetic(COMMAND::EQ);
    }
}

/**
 * term:
 *  integerConstant | stringConstant | keywordConstant | varName | varName '[' expression ']' |
 *  '(' expression ')' | (unaryOp term) | subroutineCall
 * subroutineCall: 
 *  subroutineName '(' expressionList ')' | 
 *  (className | varName) '.' subroutineName '(' expressionList ')'
 */
void CompilationEngine::compileTerm(bool isSubroutineCall) {
    TOKENTYPES tokenType;
    // integerConstant | stringConstant | keywordConstant 
    tokenType = tokenizer.peekType();
    if (tokenType == TOKENTYPES::INT_CONST || tokenType == TOKENTYPES::STRING_CONST ||
        tokenType == TOKENTYPES::KEYWORD)
    {
        std::string constantToken = tokenizer.advance(); 
        if (tokenType == TOKENTYPES::INT_CONST) {
            // push constant index
            writer.writePush(SEGMENT::CONSTANT, std::stoul(constantToken));
        }
        else if (tokenType == TOKENTYPES::STRING_CONST) {
            // push stringConst.size();
            writer.writePush(SEGMENT::CONSTANT, constantToken.size());
            // call String.new 1; (return baseAddress)
            writer.writeCall("String.new", 1);
            for (std::size_t i = 0; i < constantToken.size(); ++i) {
                // (for i=1,...,stringConst.size()) push stringConst[i];
                writer.writePush(
                    SEGMENT::CONSTANT, 
                    static_cast<std::size_t>(constantToken.at(i))
                );
                // call String.appendChar 2; (return baseAddress)
                writer.writeCall("String.appendChar", 2);
            }
        }
        else {
            // push constant true; -> push constant 1; neg;
            if (constantToken == "true") {
                writer.writePush(SEGMENT::CONSTANT, 1);
                writer.writeArithmetic(COMMAND::NEG);
            }
            // push constant false/null; -> push constant 0;
            else if (constantToken == "false" || constantToken == "null") {
                writer.writePush(SEGMENT::CONSTANT, 0);
            }
            // push this; -> push pointer 0;
            else if (constantToken == "this") {
                writer.writePush(SEGMENT::POINTER, 0);
            }
            else {
                std::cerr << "Error in compileTerm: Unexpected keywordConstant. " << std::endl;
                std::exit(EXIT_FAILURE);
            }
        }
    }
    // '(' expression ')'
    else if (tokenizer.peek() == "(") {
        process("(");
        compileExpression();
        process(")");
    }
    // unaryOp term
    else if (tokenizer.peek() == "-" || tokenizer.peek() == "~") {
        std::string unaryOp = tokenizer.advance(); // unaryOp; '-' | '~'

        // push (unaryOp term); -> push term; unaryOp;
        compileTerm();
        if (unaryOp == "-") writer.writeArithmetic(COMMAND::NEG);
        else writer.writeArithmetic(COMMAND::NOT);
    }
    // varName | varName '[' expression ']' | subroutineCall
    else {
        std::string name = tokenizer.advance(); // varName or subroutineName or className

        // varName '[' expression ']'
        if (tokenizer.peek() == "[") {
            // push (varName '[ expression ']'); -> push *(varName + expression);
            // -> push varName; push expression; add; pop pointer 1; push that 0;
            writer.writePush(symbolTable.segmentOf(name), symbolTable.indexOf(name));
            process("[");
            compileExpression();
            process("]");
            writer.writeArithmetic(COMMAND::ADD);
            writer.writePop(SEGMENT::POINTER, 1);
            writer.writePush(SEGMENT::THAT, 0);
        }
        // subroutineCall: subroutineName '(' expressionList ')'
        else if (tokenizer.peek() == "(") {
            // push this; -> push pointer 0;
            writer.writePush(SEGMENT::POINTER, 0);
            // push subroutineName '(' expressionList ')';
            // -> (for i=1,...,nArgsInExpressionList) push expression(i); 
            process("(");
            std::size_t nArgsInExpressionList = compileExpressionList();
            process(")");
            // call classNameOfThisObject.subroutineName (nArgs + 1);
            writer.writeCall(this->className + "." + name, nArgsInExpressionList + 1);
        }
        // subroutineCall: (className | varName) '.' subroutineName '(' expressionList ')'
        else if (tokenizer.peek() == ".") {
            std::string classOrVarName = name;
            // Distinguish varName.subroutineName (method) from className.subroutineName (function/constructor)
            bool isVarName = (symbolTable.kindOf(classOrVarName) != VARIABLETYPE::NONE);
            if (isVarName) {
                // push varName; -> push local index;
                writer.writePush(symbolTable.segmentOf(classOrVarName), symbolTable.indexOf(classOrVarName));
            }
            process(".");
            std::string subroutineName = tokenizer.advance(); // subroutineName
            // push subroutineName '(' expressionList ')';
            // -> (for i=1,...,nArgsInExpressionList) push expression(i);
            process("(");
            std::size_t nArgsInExpressionList = compileExpressionList();
            process(")");
            if (isVarName) {
                // call classNameOf(varName).subroutineName (nArgsInExpressionList + 1);
                writer.writeCall(symbolTable.typeOf(classOrVarName) + "." + subroutineName, nArgsInExpressionList + 1);
            }
            else {
                // call className.subroutineName nArgsInExpressionList;
                writer.writeCall(classOrVarName + "." + subroutineName, nArgsInExpressionList);
            }
        }
        // varName
        else {
            // push varName; -> push local index;
            writer.writePush(symbolTable.segmentOf(name), symbolTable.indexOf(name));
        }
    }
}

/**
 * expressionList: (expression (',' expression)* )?
 */
std::size_t CompilationEngine::compileExpressionList() {
    std::size_t count = 0;
    if (
        tokenizer.peekType() == TOKENTYPES::INT_CONST ||
        tokenizer.peekType() == TOKENTYPES::STRING_CONST ||
        tokenizer.peekType() == TOKENTYPES::KEYWORD ||
        tokenizer.peekType() == TOKENTYPES::IDENTIFIER ||
        tokenizer.peek() == "(" || tokenizer.peek() == "-" || tokenizer.peek() == "~"
    ) {
        compileExpression();
        ++count;
    }
    while (tokenizer.peek() == ",") {
        process(",");
        compileExpression();
        ++count;
    }
    return count;
}

void CompilationEngine::process(const std::string& str) {
    if (tokenizer.hasMoreTokens()) {
        tokenizer.advance();
        if (tokenizer.currentTokenStr() != str) {
            std::cout << tokenizer.currentTokenStr() << ", " << str << std::endl;
            std::cerr << "Error: Syntax error. " << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
}