#include "JackTokenizer.h"
#include "CompilationEngine.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
    std::vector<fs::directory_entry> jackfilepathFs;
    std::vector<std::string> jackfilepathStr;
    std::string filepath, jackfilename;
    if (argc < 2) {
        std::cout 
        << "Usage:" << std::endl
        << "    - JackAnalyzer <jackFile> " << std::endl
        << "    - JackAnalyzer <directory> " << std::endl;
        return 0;
    }
    if (!fs::is_directory(argv[1])) {
        CompilationEngine compilationEngine(argv[1]);
        compilationEngine.compileClass();
    }
    else {
        for (const fs::directory_entry& x : fs::directory_iterator(argv[1])) {
            if (!x.is_directory()) {
                if (x.path().string().substr(x.path().string().size() - 5) == ".jack") {
                    jackfilepathFs.push_back(x);
                }
            }
        }
        for (const fs::directory_entry& f : jackfilepathFs) {
            jackfilename = f.path().stem();
            if (!isupper(jackfilename.at(0))) {
                std::cerr << "Error: The first letter of input jack file must be upper case. " << std::endl;
                std::exit(EXIT_FAILURE);
            }
            jackfilepathStr.push_back(f.path().string());
        }
        for (const std::string& inputjackFile : jackfilepathStr) {
            CompilationEngine compilationEngine(inputjackFile);
            compilationEngine.compileClass();
        }
    }
}