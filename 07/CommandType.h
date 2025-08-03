#ifndef ___COMMANDTYPE_H___
#define ___COMMANDTYPE_H___

namespace CommandType {
    enum COMMAND_TYPE {
        C_ARITHMETIC,
        C_PUSH,
        C_POP,
        C_LABEL,
        C_GOTO,
        C_IF,
        C_FUNCTION,
        C_RETURN,
        C_CALL
    };
}

#endif