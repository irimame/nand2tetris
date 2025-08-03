#ifndef ___CODE_H___
#define ___CODE_H___

#include <string>

class Code {
public:
    static const std::string dest(const std::string &destMnemonic);
    static const std::string comp(const std::string &compMnemonic);
    static const std::string jump(const std::string &jumpMnemonic);
};

#endif