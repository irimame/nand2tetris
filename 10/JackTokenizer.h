#ifndef ___JACKTOKENIZER_H___
#define ___JACKTOKENIZER_H___

#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <map>

class JackTokenizer {
public:
    enum TOKENTYPES {
        KEYWORD,
        SYMBOL,
        IDENTIFIER,
        INT_CONST,
        STRING_CONST
    };

    enum KEYWORDS {
        CLASS,
        METHOD,
        FUNCTION,
        CONSTRUCTOR,
        INT,
        BOOLEAN,
        CHAR,
        VOID,
        VAR,
        STATIC,
        FIELD,
        LET,
        DO,
        IF,
        ELSE,
        WHILE,
        RETURN,
        TRUE,
        FALSE,
        KW_NULL,
        THIS
    };

    JackTokenizer(const std::string& inputJackFile);
    static std::string removeComments(std::string& rawText);
    static std::set<char> symbols;
    static std::set<std::string> keywords;
    static std::map<std::string, KEYWORDS> keywordMap;
    void tokenize();
    bool hasMoreTokens();
    void advance();
    const std::string& currentTokenStr();
    const std::string& peek();
    const TOKENTYPES peekType();
    const TOKENTYPES tokenType();
    const std::string& keyword();
    const char symbol();
    const std::string& identifier();
    const int intVal();
    const std::string& stringVal();

private:
    std::ifstream ifs;
    std::string trimmedText;
    std::vector<std::pair<std::string, TOKENTYPES>> token;
    int tokenIndex;
};

#endif