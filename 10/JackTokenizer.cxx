#include "JackTokenizer.h"
#include <iostream>
#include <filesystem>

std::set<char> JackTokenizer::symbols
{
    '{', '}', '(', ')', '[', ']', '.', ',', ';', '+', '-', '*', '/', 
    '&', '|', '<', '>', '=', '~'
};

std::set<std::string> JackTokenizer::keywords
{
    "class", "constructor", "function", "method", "field", "static",
    "var", "int", "char", "boolean", "void", "true", "false", "null",
    "this", "let", "do", "if", "else", "while", "return"
};

std::map<std::string, JackTokenizer::KEYWORDS> JackTokenizer::keywordMap
{
    {"class", KEYWORDS::CLASS},
    {"constructor", KEYWORDS::CONSTRUCTOR},
    {"function", KEYWORDS::FUNCTION},
    {"method", KEYWORDS::METHOD},
    {"field", KEYWORDS::FIELD},
    {"static", KEYWORDS::STATIC},
    {"var", KEYWORDS::VAR},
    {"int", KEYWORDS::INT},
    {"char", KEYWORDS::CHAR},
    {"boolean", KEYWORDS::BOOLEAN},
    {"void", KEYWORDS::VOID},
    {"true", KEYWORDS::TRUE},
    {"false", KEYWORDS::FALSE},
    {"null", KEYWORDS::KW_NULL},
    {"this", KEYWORDS::THIS},
    {"let", KEYWORDS::LET},
    {"do", KEYWORDS::DO},
    {"if", KEYWORDS::IF},
    {"else", KEYWORDS::ELSE},
    {"while", KEYWORDS::WHILE},
    {"return", KEYWORDS::RETURN}
};

JackTokenizer::JackTokenizer(const std::string& inputJackFile) 
: tokenIndex(-1)
{
    std::filesystem::path p(inputJackFile);
    std::string jackFilename = p.stem();
    if (inputJackFile.substr(inputJackFile.size() - 5, 5) != ".jack") {
        std::cerr << "Error: Please specify .jack file. " << std::endl;
        std::exit(EXIT_FAILURE);
    }
    ifs = std::ifstream(inputJackFile);
    if (!ifs) {
        std::cerr << "Error: Could not open the input jack file. " << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::istreambuf_iterator<char> it(ifs);
    std::istreambuf_iterator<char> last;
    std::string rawText(it, last);
    trimmedText = JackTokenizer::removeComments(rawText);
}

std::string JackTokenizer::removeComments(std::string& rawText) {
    std::string tmpText, trimmedText;
    std::size_t lenRawText, lenTmpText;
    bool inBlockComment = false;
    bool inLineComment = false;
    size_t i = 0;
    lenRawText = rawText.length();
    // Convert CR/CRLF to LF and horizontal tab to whitespace
    while (i < lenRawText) {
        if (rawText.at(i) == '\r') {
            tmpText += '\n';
            if (i+1 < lenRawText && rawText.at(i+1) == '\n') i += 2;
            else ++i;
        }
        else if (rawText.at(i) == '\t') {
            tmpText += " ";
            ++i;
        }
        else {
            tmpText += rawText.at(i++);
        }
    }
    // Remove comments 
    lenTmpText = tmpText.length();
    i = 0;
    while (i < lenTmpText) {
        // Comment line
        if (!inBlockComment && !inLineComment && i+1 < lenTmpText && tmpText.at(i) == '/' && tmpText.at(i+1) == '/') {
            inLineComment = true;
            i += 2;
        }
        // Beginning of a comment block
        else if (!inBlockComment && !inLineComment && i+1 < lenTmpText && tmpText.at(i) == '/' && tmpText.at(i+1) == '*') {
            inBlockComment = true;
            i += 2;
        }
        // End of block a comment block
        else if (inBlockComment && i+1 < lenTmpText && tmpText.at(i) == '*' && tmpText.at(i+1) == '/') {
            inBlockComment = false;
            i += 2;
        }
        // LF
        else if (inLineComment && tmpText.at(i) == '\n') {
            inLineComment = false;
            trimmedText += tmpText.at(i++);
        }
        // Character out of a comment section
        else if (!inBlockComment && !inLineComment) {
            trimmedText += tmpText.at(i++);
        }
        // Character in a comment section
        else {
            ++i;
        }
    }
    return trimmedText;
}

void JackTokenizer::tokenize() {
    std::size_t i = 0;
    while (i < trimmedText.size()) {
        if (trimmedText.at(i) == ' ' || trimmedText.at(i) == '\n') {
            ++i;
        }
        else if (symbols.find(trimmedText.at(i)) != symbols.end()) {
            token.push_back(std::make_pair(std::string(1, trimmedText.at(i)), TOKENTYPES::SYMBOL));
            ++i;
        }
        else if (trimmedText.at(i) == '"') {
            std::string strConst;
            ++i;
            while (trimmedText.at(i) != '"') strConst += trimmedText.at(i++);
            token.push_back(std::make_pair(strConst, TOKENTYPES::STRING_CONST));
            ++i;
        }
        else if (isdigit(trimmedText.at(i))) {
            std::string intConst;
            while (isdigit(trimmedText.at(i))) intConst += trimmedText.at(i++);
            token.push_back(std::make_pair(intConst, TOKENTYPES::INT_CONST));
        }
        else if (isalpha(trimmedText.at(i)) || trimmedText.at(i) == '_') {
            std::string identifier;
            while ( !(trimmedText.at(i) == ' ' || 
                    symbols.find(trimmedText.at(i)) != symbols.end()) ) 
            {
                identifier += trimmedText.at(i++);
            }
            if (keywords.find(identifier) != keywords.end()) {
                token.push_back(std::make_pair(identifier, TOKENTYPES::KEYWORD));
            }
            else {
                token.push_back(std::make_pair(identifier, TOKENTYPES::IDENTIFIER));
            }
        }
        else {
            std::cerr << "Error: Unknown type token appears at index " << i << ". " << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
}

bool JackTokenizer::hasMoreTokens() {
    if (this->tokenIndex + 1 < this->token.size()) return true;
    else return false;
}

void JackTokenizer::advance() {
    if (hasMoreTokens()) ++this->tokenIndex;
}

const std::string& JackTokenizer::currentTokenStr() {
    return this->token.at(this->tokenIndex).first;
}

const std::string& JackTokenizer::peek() {
    return this->token.at(this->tokenIndex + 1).first;
}

const JackTokenizer::TOKENTYPES JackTokenizer::peekType() {
    return this->token.at(this->tokenIndex + 1).second;
}

const JackTokenizer::TOKENTYPES JackTokenizer::tokenType() {
    return this->token.at(this->tokenIndex).second;
}

const std::string& JackTokenizer::keyword() {
    return this->token.at(this->tokenIndex).first;
}

const char JackTokenizer::symbol() {
    return this->token.at(this->tokenIndex).first.at(0);
}

const std::string& JackTokenizer::identifier() {
    return this->token.at(this->tokenIndex).first;
}

const int JackTokenizer::intVal() {
    return std::stoi(this->token.at(this->tokenIndex).first);
}

const std::string& JackTokenizer::stringVal() {
    return this->token.at(this->tokenIndex).first;
}