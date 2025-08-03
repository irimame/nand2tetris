#include "CodeWriter.h"
#include <iostream>
#include <cstdlib>
#include <filesystem>

namespace {
    std::string segmentSymbol(const std::string& segment, std::size_t index) {
        if (segment == "local") return "@LCL";
        else if (segment == "argument") return "@ARG";
        else if (segment == "this") return "@THIS";
        else if (segment == "that") return "@THAT";
        else if (segment == "temp") return "@5";
        else if (segment == "pointer") return "@3";
        else {
            std::cerr << "Unknown segment '" << segment << "'. " << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
}

CodeWriter::CodeWriter(const std::string& inputVMFile) 
: compCount(0) {
    std::string outputAsmFile = inputVMFile.substr(0, inputVMFile.size() - 3) + ".asm";
    ofs = std::ofstream(outputAsmFile);
    if (!ofs) {
        std::cerr << "Error: Could not open the output asm file. " << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::filesystem::path p(inputVMFile);
    VMFilename = p.stem();
}

void CodeWriter::writeArithmetic(const std::string& command) {
    ofs << "// " << command << std::endl;
    // add, sub, and, or
    if (command == "add" || command == "sub" ||
        command == "and" || command == "or") {
        popxy();
        ofs << "@R14"   << std::endl
            << "D=M"    << std::endl
            << "@R13"   << std::endl;
        if (command == "add") {
            ofs << "D=D+M" << std::endl;
        }
        else if (command == "sub") {
            ofs << "D=D-M" << std::endl;
        }
        else if (command == "and") {
            ofs << "D=D&M" << std::endl;
        }
        else {
            ofs << "D=D|M" << std::endl;
        }
        push();
    }
    // neg, not
    else if (command == "neg" || command == "not") {
        popy();
        ofs << "@R13"   << std::endl
            << "D=M"    << std::endl;
        if (command == "neg") {
            ofs << "D=-D" << std::endl;
        }
        else {
            ofs << "D=!D" << std::endl;
        }
        push();
    }
    // eq, gt, lt
    else if (command == "eq" || command == "gt" || command == "lt") {
        popxy();
        ofs << "@R14"       << std::endl
            << "D=M"        << std::endl
            << "@R13"       << std::endl
            << "D=D-M"      << std::endl
            << "@COMPTRUE" << this->compCount << std::endl;
        if (command == "eq") {
            ofs << "D;JEQ" << std::endl;
        }
        else if (command == "gt") {
            ofs << "D;JGT" << std::endl;
        }
        else {
            ofs << "D;JLT" << std::endl;
        }
        // false
        ofs << "D=0" << std::endl;
        push();
        ofs << "@COMPEND" << this->compCount << std::endl
            << "0;JMP"    << std::endl;
        // true
        ofs << "(COMPTRUE" << this->compCount << ")" << std::endl
            << "D=-1"      << std::endl;
        push();
        // end
        ofs << "(COMPEND" << this->compCount << ")" << std::endl;
        ++(this->compCount);
    }
    else {
        std::cerr << "Error: Unknown command '" << command << "'. " << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void CodeWriter::writePushPop(const CommandType::COMMAND_TYPE commandType, const std::string& segment, const std::size_t index) {
    if (commandType == CommandType::COMMAND_TYPE::C_PUSH) {
        ofs << "// push " << segment << " " << index << std::endl;
        if (segment == "constant") {
            ofs << "@" << index << std::endl
                << "D=A" << std::endl;
        }
        else if (segment == "static") {
            ofs << "@" << this->VMFilename << "." << index << std::endl
                << "D=M" << std::endl;
        }
        else  {
            ofs << segmentSymbol(segment, index) << std::endl;
                if (segment == "temp" || segment == "pointer") ofs << "D=A" << std::endl;
                else ofs << "D=M" << std::endl;
                ofs << "@" << index << std::endl
                    << "A=D+A" << std::endl
                    << "D=M" << std::endl;
        }
        push();
    }
    else if (commandType == CommandType::COMMAND_TYPE::C_POP) {
        ofs << "// pop " << segment << " " << index << std::endl;
        pop();
        if (segment == "static") {
            ofs << "@" << this->VMFilename << "." << index << std::endl
                << "M=D" << std::endl;
        }
        else {
            ofs << "@R13" << std::endl
                << "M=D" << std::endl
                << segmentSymbol(segment, index) << std::endl;
            if (segment == "temp" || segment == "pointer") ofs << "D=A" << std::endl;
            else ofs << "D=M" << std::endl;
            ofs << "@" << index << std::endl
                << "D=D+A" << std::endl
                << "@R14" << std::endl
                << "M=D" << std::endl
                << "@R13" << std::endl
                << "D=M" << std::endl
                << "@R14" << std::endl
                << "A=M" << std::endl
                << "M=D" << std::endl;
        }
    }
    else {
        std::cerr << "Error: Unexpected commandType '" << commandType << "'. " << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void CodeWriter::close() {
    ofs << "(END)" << std::endl
        << "@END" << std::endl
        << "0;JMP" << std::endl;
    if (ofs.is_open()) ofs.close();
}

void CodeWriter::popxy() {
    pop();
    // RAM[R13] <- y
    ofs << "@R13"   << std::endl
        << "M=D"    << std::endl;
    pop();
    // RAM[R14] <- x
    ofs << "@R14"   << std::endl
        << "M=D"    << std::endl;
}

void CodeWriter::popy() {
    pop();
    // RAM[R13] <- y
    ofs << "@R13"   << std::endl 
        << "M=D"    << std::endl; 
}

void CodeWriter::push() {
    ofs << "@SP"    << std::endl
        << "A=M"    << std::endl
        << "M=D"    << std::endl
        << "@SP"    << std::endl
        << "M=M+1"  << std::endl;
}

void CodeWriter::pop() {
    ofs << "@SP"    << std::endl
        << "M=M-1"  << std::endl
        << "A=M"    << std::endl
        << "D=M"    << std::endl;
}