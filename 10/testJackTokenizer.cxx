#include "JackTokenizer.h"
#include <iostream>
#include <cstdlib>

int main() {
    std::string input = "./Square/Main.jack";
    std::string output_getTrimmedText = "./test_getTrimmedText.txt";
    std::string output_getToken = "./test_getToken.txt";
    JackTokenizer tokenizer(input);
    std::string trimmedText = tokenizer.test_getTrimmedText();

    std::ofstream ofs(output_getTrimmedText);
    if (!ofs) {
        std::cerr << "Error: Cannot open" << output_getTrimmedText << ". " << std::endl;
        std::exit(EXIT_FAILURE);
    }
    ofs << trimmedText;
    if (ofs.is_open()) ofs.close();

    tokenizer.tokenize();
    std::ofstream ofss(output_getToken);
    if (!ofss) {
        std::cerr << "Error: Cannot open" << output_getToken << ". " << std::endl;
        std::exit(EXIT_FAILURE);
    }
    tokenizer.test_printToken(ofss);
    if (ofss.is_open()) ofss.close();
}