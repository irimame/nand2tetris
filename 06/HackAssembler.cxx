#include "Parser.h"
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: HackAssembler <asmfilename> " << std::endl;
        return 0;
    }
    Parser parser(argv[1]);
    parser.parse();
    return 0;
}