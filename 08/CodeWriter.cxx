#include "CodeWriter.h"
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <libgen.h>

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
    
    void pushD(std::ostream &ofs) {
        ofs << "@SP"    << std::endl
            << "A=M"    << std::endl
            << "M=D"    << std::endl
            << "@SP"    << std::endl
            << "M=M+1"  << std::endl;
    }

    void popD(std::ostream &ofs) {
        ofs << "@SP"    << std::endl
            << "M=M-1"  << std::endl
            << "A=M"    << std::endl
            << "D=M"    << std::endl;
    }

    void popxy(std::ostream &ofs) {
        popD(ofs);
        // RAM[R13] <- y
        ofs << "@R13"   << std::endl
            << "M=D"    << std::endl;
        popD(ofs);
        // RAM[R14] <- x
        ofs << "@R14"   << std::endl
            << "M=D"    << std::endl;
    }

    void popy(std::ostream &ofs) {
        popD(ofs);
        // RAM[R13] <- y
        ofs << "@R13"   << std::endl 
            << "M=D"    << std::endl; 
    }

    void pushConstant(std::ostream& ofs, const std::size_t val) {
        ofs << "@" << val   << std::endl
            << "D=A"        << std::endl;
        pushD(ofs);
    }
}

std::string CodeWriter::outputAsmFile = "";

void CodeWriter::writeBootstrapCode(std::string directory) {
    char *directoryDup = directory.data();
    char *dirBasename(directoryDup);

    dirBasename = basename(directoryDup);
    outputAsmFile = directory + "/" + std::string(dirBasename) + ".asm";
    std::ofstream ofsinit(outputAsmFile);

    if (!ofsinit) {
        std::cerr << "Error: Could not open the output asm file. " << std::endl;
        std::exit(EXIT_FAILURE);
    }
    // set SP to 256
    ofsinit << "@256" << std::endl
            << "D=A" << std::endl
            << "@SP" << std::endl
            << "M=D" << std::endl;
    // call Sys.init
    CodeWriter::writeCallSysinit(ofsinit, 0);
    if (ofsinit.is_open()) ofsinit.close();
}

void CodeWriter::writeCallSysinit(std::ofstream& ofs, const std::size_t nArgs) {
    ofs << "// call Sys.init" << std::endl;
    // push returnAddress
    ofs << "@" << "$ret." << std::endl
        << "D=A" << std::endl;
    pushD(ofs);
    // push LCL
    ofs << "@LCL" << std::endl
        << "D=M" << std::endl;
    pushD(ofs);
    // push ARG
    ofs << "@ARG" << std::endl
        << "D=M" << std::endl;
    pushD(ofs);
    // push THIS
    ofs << "@THIS" << std::endl
        << "D=M" << std::endl;
    pushD(ofs);
    // push THAT
    ofs << "@THAT" << std::endl
        << "D=M" << std::endl;
    pushD(ofs);
    // ARG = SP - 5 - nArgs
    ofs << "@SP" << std::endl
        << "D=M" << std::endl
        << "@5"  << std::endl
        << "D=D-A" << std::endl
        << "@" << nArgs << std::endl
        << "D=D-A" << std::endl
        << "@ARG" << std::endl
        << "M=D" << std::endl;
    // LCL = SP
    ofs << "@SP" << std::endl
        << "D=M" << std::endl
        << "@LCL" << std::endl
        << "M=D" << std::endl;
    // goto f
    ofs << "@" << "Sys.init" << std::endl
        << "0;JMP" << std::endl;
    // (returnAddress)
    ofs << "(" << "$ret." << ")" << std::endl;
}

CodeWriter::CodeWriter(const std::string& inputVMFile) 
: compCount(0), returnAddCount(0), currentFunctionName("") {
    ofs = std::ofstream(outputAsmFile, std::ios::app);
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
        popxy(ofs);
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
        pushD(ofs);
    }
    // neg, not
    else if (command == "neg" || command == "not") {
        popy(ofs);
        ofs << "@R13"   << std::endl
            << "D=M"    << std::endl;
        if (command == "neg") {
            ofs << "D=-D" << std::endl;
        }
        else {
            ofs << "D=!D" << std::endl;
        }
        pushD(ofs);
    }
    // eq, gt, lt
    else if (command == "eq" || command == "gt" || command == "lt") {
        popxy(ofs);
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
        pushD(ofs);
        ofs << "@COMPEND" << this->compCount << std::endl
            << "0;JMP"    << std::endl;
        // true
        ofs << "(COMPTRUE" << this->compCount << ")" << std::endl
            << "D=-1"      << std::endl;
        pushD(ofs);
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
        pushD(ofs);
    }
    else if (commandType == CommandType::COMMAND_TYPE::C_POP) {
        ofs << "// pop " << segment << " " << index << std::endl;
        popD(ofs);
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

void CodeWriter::writeLabel(const std::string& label) {
    ofs << "// label " << currentFunctionName << "$" << label << std::endl;
    ofs << "(" << currentFunctionName << "$" << label << ")" << std::endl;
}

void CodeWriter::writeGoto(const std::string& label) {
    ofs << "// goto " << currentFunctionName << "$" << label << std::endl;
    ofs << "@" << currentFunctionName << "$" << label << std::endl
        << "0;JMP" << std::endl;
}

void CodeWriter::writeIf(const std::string& label) {
    ofs << "// if-goto " << currentFunctionName << "$" << label << std::endl;
    popD(ofs);
    ofs << "@" << currentFunctionName << "$" << label << std::endl
        << "D;JNE" << std::endl;
}

void CodeWriter::writeFunction(const std::string& functionName, const std::size_t nVars) {
    ofs << "// function " << functionName << " " << nVars << std::endl;
    ofs << "(" << functionName << ")" << std::endl;
    for (std::size_t i = 0; i < nVars; ++i) pushConstant(ofs, 0);
}

void CodeWriter::writeCall(const std::string& functionName, const std::size_t nArgs) {
    ofs << "// call " << functionName << " " << nArgs << std::endl;
    // push returnAddress
    ofs << "@" << currentFunctionName << "$ret." << returnAddCount << std::endl
        << "D=A" << std::endl;
    pushD(ofs);
    // push LCL
    ofs << "@LCL" << std::endl
        << "D=M" << std::endl;
    pushD(ofs);
    // push ARG
    ofs << "@ARG" << std::endl
        << "D=M" << std::endl;
    pushD(ofs);
    // push THIS
    ofs << "@THIS" << std::endl
        << "D=M" << std::endl;
    pushD(ofs);
    // push THAT
    ofs << "@THAT" << std::endl
        << "D=M" << std::endl;
    pushD(ofs);
    // ARG = SP - 5 - nArgs
    ofs << "@SP" << std::endl
        << "D=M" << std::endl
        << "@5"  << std::endl
        << "D=D-A" << std::endl
        << "@" << nArgs << std::endl
        << "D=D-A" << std::endl
        << "@ARG" << std::endl
        << "M=D" << std::endl;
    // LCL = SP
    ofs << "@SP" << std::endl
        << "D=M" << std::endl
        << "@LCL" << std::endl
        << "M=D" << std::endl;
    // goto f
    ofs << "@" << functionName << std::endl
        << "0;JMP" << std::endl;
    // (returnAddress)
    ofs << "(" << currentFunctionName << "$ret." << returnAddCount++ << ")" << std::endl;
}

void CodeWriter::writeReturn() {
    ofs << "// return" << std::endl;
    // frame(R13) = LCL
    ofs << "@LCL" << std::endl
        << "D=M" << std::endl
        << "@R13" << std::endl
        << "M=D" << std::endl;
    // retAddr(R14) = *(frame - 5)
    ofs << "@R13" << std::endl
        << "D=M" << std::endl
        << "@5" << std::endl
        << "A=D-A" << std::endl
        << "D=M" << std::endl
        << "@R14" << std::endl
        << "M=D" << std::endl;
    // *ARG = pop()
    popD(ofs);
    ofs << "@ARG" << std::endl
        << "A=M" << std::endl
        << "M=D" << std::endl;
    // SP = ARG + 1
    ofs << "@ARG" << std::endl
        << "D=M" << std::endl
        << "@SP" << std::endl
        << "M=D+1" << std::endl;
    // THAT = *(frame - 1)
    ofs << "@R13" << std::endl
        << "D=M" << std::endl
        << "@1" << std::endl
        << "A=D-A" << std::endl
        << "D=M" << std::endl
        << "@THAT" << std::endl
        << "M=D" << std::endl;
    // THIS = *(frame - 2)
    ofs << "@R13" << std::endl
        << "D=M" << std::endl
        << "@2" << std::endl
        << "A=D-A" << std::endl
        << "D=M" << std::endl
        << "@THIS" << std::endl
        << "M=D" << std::endl;
    // ARG = *(frame - 3)
    ofs << "@R13" << std::endl
        << "D=M" << std::endl
        << "@3" << std::endl
        << "A=D-A" << std::endl
        << "D=M" << std::endl
        << "@ARG" << std::endl
        << "M=D" << std::endl;
    // LCL = *(frame - 4)
    ofs << "@R13" << std::endl
        << "D=M" << std::endl
        << "@4" << std::endl
        << "A=D-A" << std::endl
        << "D=M" << std::endl
        << "@LCL" << std::endl
        << "M=D" << std::endl;
    // goto retAddr (R14)
    ofs << "@R14" << std::endl
        << "A=M" << std::endl
        << "0;JMP" << std::endl;
}

void CodeWriter::setCurrentFunctionName(const std::string& functionName) {
    currentFunctionName = functionName;
}

void CodeWriter::close() {
    ofs << "(END)" << std::endl
        << "@END" << std::endl
        << "0;JMP" << std::endl;
    if (ofs.is_open()) ofs.close();
}