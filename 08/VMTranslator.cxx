#include "Parser.h"
#include <iostream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
    std::vector<fs::directory_entry> vmfilepathFs;
    std::vector<std::string> vmfilepathStr;
    std::string filepath, vmfilename;
    if (argc < 2) {
        std::cout << "Usage: VMTranslator <directory> " << std::endl;
        return 0;
    }
    CodeWriter::writeBootstrapCode(argv[1]);
    for (const fs::directory_entry& x : fs::directory_iterator(argv[1])) {
        if (!x.is_directory()) {
            if (x.path().string().substr(x.path().string().size() - 3) == ".vm") {
                vmfilepathFs.push_back(x);
            }
        }
    }
    for (const fs::directory_entry& f : vmfilepathFs) {
        vmfilename = f.path().stem();
        if (!isupper(vmfilename.at(0))) {
            std::cerr << "Error: The first letter of input vm file must be upper case. " << std::endl;
            std::exit(EXIT_FAILURE);
        }
        vmfilepathStr.push_back(f.path().string());
    }
    for (const std::string& inputVMFile : vmfilepathStr) {
        Parser parser(inputVMFile);
        parser.parse();
    }
    return 0;
}