#ifndef ___TOKENTYPEENUM_H___
#define ___TOKENTYPEENUM_H___

enum class TOKENTYPES {
    KEYWORD,
    SYMBOL,
    IDENTIFIER,
    INT_CONST,
    STRING_CONST
};

enum class KEYWORDS {
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

enum class VARIABLETYPE {
    STATIC,
    FIELD,
    ARG,
    VAR,
    NONE
};

enum class SEGMENT {
    CONSTANT,
    ARGUMENT,
    LOCAL,
    STATIC,
    THIS,
    THAT,
    POINTER,
    TEMP
};

enum class COMMAND {
    ADD,
    SUB,
    NEG,
    EQ,
    GT,
    LT,
    AND,
    OR,
    NOT
};

#endif