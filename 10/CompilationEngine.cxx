#include "CompilationEngine.h"
#include <iostream>

CompilationEngine::CompilationEngine(const std::string& inputJackFile)
: tokenizer(inputJackFile)
{
    tokenizer.tokenize();
    ofs = std::ofstream(inputJackFile.substr(0, inputJackFile.size() - 5) + "CTest.xml");
    if (!ofs) {
        std::cerr << "Error: Could not open the output XML file. " << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

/**
 * class: 'class' className '{' classVarDec* subroutineDec* '}'
 */
void CompilationEngine::compileClass() {
    ofs << "<class>" << std::endl;
    process("class");
    tokenizer.advance(); printXMLToken(); // className
    process("{");
    while (tokenizer.peek() == "static" || tokenizer.peek() == "field") {
        compileClassVarDec();
    }
    while (tokenizer.peek() == "constructor" || tokenizer.peek() == "function" || tokenizer.peek() == "method") {
        compileSubroutineDec();
    }
    process("}");
    ofs << "</class>" << std::endl;
}

/**
 * classVarDec: ('static' | 'field') type varName (',' varName)* ';'
 */
void CompilationEngine::compileClassVarDec() {
    ofs << "<classVarDec>" << std::endl;
    tokenizer.advance(); printXMLToken(); // 'static' or 'field'
    tokenizer.advance(); printXMLToken(); // type
    tokenizer.advance(); printXMLToken(); // varName
    while (tokenizer.peek() == ",") {
        process(",");
        tokenizer.advance(); printXMLToken(); // varName
    }
    process(";");
    ofs << "</classVarDec>" << std::endl;
}

/**
 * subroutineDec: ('constructor' | 'function' | 'method' ) ('void' | type) subroutineName
 *                  '(' parameterList ')' subroutineBody
 */
void CompilationEngine::compileSubroutineDec() {
    ofs << "<subroutineDec>" << std::endl;
    tokenizer.advance(); printXMLToken(); // 'constructor' or 'function' or 'method'
    tokenizer.advance(); printXMLToken(); // 'void' or type
    tokenizer.advance(); printXMLToken(); // subroutineName
    process("(");
    compileParameterList();
    process(")");
    compileSubroutineBody();
    ofs << "</subroutineDec>" << std::endl;
}

/**
 * parameterList: ((type varName) (',' type varName)*)?
 */
void CompilationEngine::compileParameterList() {
    ofs << "<parameterList>" << std::endl;
    if (tokenizer.peek() == "int" || tokenizer.peek() == "char" || tokenizer.peek() == "boolean" || 
        tokenizer.peekType() == JackTokenizer::TOKENTYPES::IDENTIFIER) 
    {
        tokenizer.advance(); printXMLToken(); // type
        tokenizer.advance(); printXMLToken(); // varName
        while (tokenizer.peek() == ",") {
            process(",");
            tokenizer.advance(); printXMLToken(); // type
            tokenizer.advance(); printXMLToken(); // varName
        }
    }
    ofs << "</parameterList>" << std::endl;
}

/**
 * subroutineBody: '{' varDec* statements '}'
 */
void CompilationEngine::compileSubroutineBody() {
    ofs << "<subroutineBody>" << std::endl;
    process("{");
    while (tokenizer.peek() == "var") {
        compileVarDec();
    }
    compileStatements();
    process("}");
    ofs << "</subroutineBody>" << std::endl;
}

/**
 * varDec: 'var' type varName (',' varName)* ';'
 */
void CompilationEngine::compileVarDec() {
    ofs << "<varDec>" << std::endl;
    process("var");
    tokenizer.advance(); printXMLToken(); // type
    tokenizer.advance(); printXMLToken(); // varName
    while (tokenizer.peek() == ",") {
        process(",");
        tokenizer.advance(); printXMLToken(); // varName
    }
    process(";");
    ofs << "</varDec>" << std::endl;
}

/**
 * statements: statement*
 * statement: letStatement | ifStatement | whileStatement | doStatement | returnStatement
 */
void CompilationEngine::compileStatements() {
    ofs << "<statements>" << std::endl;
    while (tokenizer.peek() == "let" || tokenizer.peek() == "if" || tokenizer.peek() == "while" ||
            tokenizer.peek() == "do" || tokenizer.peek() == "return")
    {
        if (tokenizer.peek() == "let") compileLet();
        else if (tokenizer.peek() == "if") compileIf();
        else if (tokenizer.peek() == "while") compileWhile();
        else if (tokenizer.peek() == "do") compileDo();
        else if (tokenizer.peek() == "return") compileReturn();
    }
    ofs << "</statements>" << std::endl;
}

/**
 * letStatement: 'let' varName ('[' expression ']')? '=' expression ';'
 */
void CompilationEngine::compileLet() {
    ofs << "<letStatement>" << std::endl;
    process("let");
    tokenizer.advance(); printXMLToken(); // varName
    if (tokenizer.peek() == "[") {
        process("[");
        compileExpression();
        process("]");
    }
    process("=");
    compileExpression();
    process(";");
    ofs << "</letStatement>" << std::endl;
}

/**
 * ifStatement: 'if' '(' expression ')' '{' statements '}' ('else' '{' statements '}')?
 */
void CompilationEngine::compileIf() {
    ofs << "<ifStatement>" << std::endl;
    process("if");
    process("(");
    compileExpression();
    process(")");
    process("{");
    compileStatements();
    process("}");
    if (tokenizer.peek() == "else") {
        process("else");
        process("{");
        compileStatements();
        process("}");
    }
    ofs << "</ifStatement>" << std::endl;
}

/**
 * whileStatement: 'while' '(' expression ')' '{' statements '}'
 */
void CompilationEngine::compileWhile() {
    ofs << "<whileStatement>" << std::endl;
    process("while");
    process("(");
    compileExpression();
    process(")");
    process("{");
    compileStatements();
    process("}");
    ofs << "</whileStatement>" << std::endl;
}

/**
 * doStatement: 'do' subroutineCall ';'
 *              -> 'do' expression ';'
 */
void CompilationEngine::compileDo() {
    ofs << "<doStatement>" << std::endl;
    process("do");
    compileExpression(true);
    process(";");
    ofs << "</doStatement>" << std::endl;
}

/**
 * returnStatement: 'return' expression? ';'
 */
void CompilationEngine::compileReturn() {
    ofs << "<returnStatement>" << std::endl;
    process("return");
    if (
        tokenizer.peekType() == JackTokenizer::TOKENTYPES::INT_CONST ||
        tokenizer.peekType() == JackTokenizer::TOKENTYPES::STRING_CONST ||
        tokenizer.peekType() == JackTokenizer::TOKENTYPES::KEYWORD ||
        tokenizer.peekType() == JackTokenizer::TOKENTYPES::IDENTIFIER ||
        tokenizer.peek() == "(" || tokenizer.peek() == "-" || tokenizer.peek() == "~"
    ) {
        compileExpression();
    }
    process(";");
    ofs << "</returnStatement>" << std::endl;
}

/**
 * expression: term (op term)*
 */
void CompilationEngine::compileExpression(bool isSubroutineCall) {
    if (!isSubroutineCall) ofs << "<expression>" << std::endl;
    compileTerm(isSubroutineCall);
    while (tokenizer.peek() == "+" || tokenizer.peek() == "-" || tokenizer.peek() == "*" ||
            tokenizer.peek() == "/" || tokenizer.peek() == "&" || tokenizer.peek() == "|" ||
            tokenizer.peek() == "<" || tokenizer.peek() == ">" || tokenizer.peek() == "=")
    {
        tokenizer.advance(); printXMLToken(); // op
        compileTerm(isSubroutineCall);
    }
    if (!isSubroutineCall) ofs << "</expression>" << std::endl;
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
    if (!isSubroutineCall) ofs << "<term>" << std::endl;
    // integerConstant | stringConstant | keywordConstant 
    if (tokenizer.peekType() == JackTokenizer::TOKENTYPES::INT_CONST ||
        tokenizer.peekType() == JackTokenizer::TOKENTYPES::STRING_CONST ||
        tokenizer.peekType() == JackTokenizer::TOKENTYPES::KEYWORD)
    {
        tokenizer.advance(); printXMLToken(); 
    }
    // '(' expression ')'
    else if (tokenizer.peek() == "(") {
        process("(");
        compileExpression();
        process(")");
    }
    // unaryOp term
    else if (tokenizer.peek() == "-" || tokenizer.peek() == "~") {
        tokenizer.advance(); printXMLToken(); // unaryOp; '-' | '~'
        compileTerm();
    }
    // varName | varName '[' expression ']' | subroutineCall
    else {
        tokenizer.advance(); printXMLToken(); // varName or subroutineName or className
        // varName '[' expression ']'
        if (tokenizer.peek() == "[") {
            process("[");
            compileExpression();
            process("]");
        }
        // subroutineCall: subroutineName '(' expressionList ')'
        else if (tokenizer.peek() == "(") {
            process("(");
            compileExpressionList();
            process(")");
        }
        // subroutineCall: (className | varName) '.' subroutineName '(' expressionList ')'
        else if (tokenizer.peek() == ".") {
            process(".");
            tokenizer.advance(); printXMLToken(); // subroutineName
            process("(");
            compileExpressionList();
            process(")");
        }
    }
    if (!isSubroutineCall) ofs << "</term>" << std::endl;
}

/**
 * expressionList: (expression (',' expression)* )?
 */
std::size_t CompilationEngine::compileExpressionList() {
    std::size_t count = 0;
    ofs << "<expressionList>" << std::endl;
    if (
        tokenizer.peekType() == JackTokenizer::TOKENTYPES::INT_CONST ||
        tokenizer.peekType() == JackTokenizer::TOKENTYPES::STRING_CONST ||
        tokenizer.peekType() == JackTokenizer::TOKENTYPES::KEYWORD ||
        tokenizer.peekType() == JackTokenizer::TOKENTYPES::IDENTIFIER ||
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
    ofs << "</expressionList>" << std::endl;
    return count;
}

void CompilationEngine::process(const std::string& str) {
    if (tokenizer.hasMoreTokens()) {
        tokenizer.advance();
        if (tokenizer.currentTokenStr() == str) {
            printXMLToken();
        }
        else {
            std::cout << tokenizer.currentTokenStr() << ", " << str << std::endl;
            std::cerr << "Error: Syntax error. " << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
}

void CompilationEngine::printXMLToken() {
    if(tokenizer.tokenType() == JackTokenizer::KEYWORD) {
        ofs << "<keyword> " << tokenizer.keyword() << " </keyword>" << std::endl;
    }
    else if (tokenizer.tokenType() == JackTokenizer::SYMBOL) {
        if (tokenizer.symbol() == '<') {
            ofs << "<symbol> &lt; </symbol>" << std::endl;
        }
        else if (tokenizer.symbol() == '>') {
            ofs << "<symbol> &gt; </symbol>" << std::endl;
        }
        else if (tokenizer.symbol() == '"') {
            ofs << "<symbol> &quot; </symbol>" << std::endl;
        }
        else if (tokenizer.symbol() == '&') {
            ofs << "<symbol> &amp; </symbol>" << std::endl;
        }
        else {
            ofs << "<symbol> " << tokenizer.symbol() << " </symbol>" << std::endl;
        }
    }
    else if (tokenizer.tokenType() == JackTokenizer::IDENTIFIER) {
        ofs << "<identifier> " << tokenizer.identifier() << " </identifier>" << std::endl;
    }
    else if (tokenizer.tokenType() == JackTokenizer::INT_CONST) {
        ofs << "<integerConstant> " << tokenizer.intVal() << " </integerConstant>" << std::endl;
    }
    else if (tokenizer.tokenType() == JackTokenizer::STRING_CONST) {
        ofs << "<stringConstant> " << tokenizer.stringVal() << " </stringConstant>" << std::endl;
    }
    else {
        std::cerr << "Error: Undefined token type. " << std::endl;
        std::exit(EXIT_FAILURE);
    }
}