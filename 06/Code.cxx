#include "Code.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>

const std::string Code::dest(const std::string& destMnemonic) {
    if (destMnemonic == "null") {
        return "000";
    }
    std::string sortedDestMnemonic = std::string(destMnemonic);
    std::sort(sortedDestMnemonic.begin(), sortedDestMnemonic.end());
    if (sortedDestMnemonic == "M") {
        return "001";
    }
    else if (sortedDestMnemonic == "D") {
        return "010";
    }
    else if (sortedDestMnemonic == "DM") {
        return "011";
    }
    else if (sortedDestMnemonic == "A") {
        return "100";
    }
    else if (sortedDestMnemonic == "AM") {
        return "101";
    }
    else if (sortedDestMnemonic == "AD") {
        return "110";
    }
    else if (sortedDestMnemonic == "ADM") {
        return "111";
    }
    else {
        std::cerr << "Error: Unknown dest. mnemonic '" << destMnemonic << "'." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

const std::string Code::comp(const std::string& compMnemonic) {
    std::string a;

    if (compMnemonic.find('M') != std::string::npos) a = "1";
    else a = "0";

    if (compMnemonic == "0") {
        return a + "101010";
    }
    else if (compMnemonic == "1") {
        return a + "111111";
    }
    else if (compMnemonic == "-1") {
        return a + "111010";
    }
    else if (compMnemonic == "D") {
        return a + "001100";
    }
    else if (compMnemonic == "A" || compMnemonic == "M") {
        return a + "110000";
    }
    else if (compMnemonic == "!D") {
        return a + "001101";
    }
    else if (compMnemonic == "!A" || compMnemonic == "!M") {
        return a + "110001";
    }
    else if (compMnemonic == "-D") {
        return a + "001111";
    }
    else if (compMnemonic == "-A" || compMnemonic == "-M") {
        return a + "110011";
    }
    else if (compMnemonic == "D+1") {
        return a + "011111";
    }
    else if (compMnemonic == "A+1" || compMnemonic == "M+1") {
        return a + "110111";
    }
    else if (compMnemonic == "D-1") {
        return a + "001110";
    }
    else if (compMnemonic == "A-1" || compMnemonic == "M-1") {
        return a + "110010";
    }
    else if (compMnemonic == "D+A" || compMnemonic == "D+M") {
        return a + "000010";
    }
    else if (compMnemonic == "D-A" || compMnemonic == "D-M") {
        return a + "010011";
    }
    else if (compMnemonic == "A-D" || compMnemonic == "M-D") {
        return a + "000111";
    }
    else if (compMnemonic == "D&A" || compMnemonic == "D&M") {
        return a + "000000";
    }
    else if (compMnemonic == "D|A" || compMnemonic == "D|M") {
        return a + "010101";
    }
    else {
        std::cerr << "Error: Unknown comp. mnemonic '" << compMnemonic << "'." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

const std::string Code::jump(const std::string& jumpMnemonic) {
    if (jumpMnemonic == "null") {
        return "000";
    }
    else if (jumpMnemonic == "JGT") {
        return "001";
    }
    else if (jumpMnemonic == "JEQ") {
        return "010";
    }
    else if (jumpMnemonic == "JGE") {
        return "011";
    }
    else if (jumpMnemonic == "JLT") {
        return "100";
    }
    else if (jumpMnemonic == "JNE") {
        return "101";
    }
    else if (jumpMnemonic == "JLE") {
        return "110";
    }
    else if (jumpMnemonic == "JMP") {
        return "111";
    }
    else {
        std::cerr << "Error: Unknown jump mnemonic '" << jumpMnemonic << "'." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}