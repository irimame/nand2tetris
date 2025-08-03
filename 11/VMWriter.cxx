#include "VMWriter.h"
#include <iostream>

VMWriter::VMWriter(const std::string& inputJackFile) {
    ofs = std::ofstream(inputJackFile.substr(0, inputJackFile.size() - 5) + ".vm");
    if (!ofs) {
        std::cerr << "Error: Could not open the output VM file. " << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void VMWriter::writePush(const SEGMENT segment, const std::size_t index) {
    switch(segment) {
        case SEGMENT::CONSTANT:
            ofs << "push constant " << index << std::endl;
            break;
        case SEGMENT::ARGUMENT:
            ofs << "push argument " << index << std::endl;
            break;
        case SEGMENT::LOCAL:
            ofs << "push local " << index << std::endl;
            break;
        case SEGMENT::STATIC:
            ofs << "push static " << index << std::endl;
            break;
        case SEGMENT::THIS:
            ofs << "push this " << index << std::endl;
            break;
        case SEGMENT::THAT:
            ofs << "push that " << index << std::endl;
            break;
        case SEGMENT::POINTER:
            ofs << "push pointer " << index << std::endl;
            break;
        case SEGMENT::TEMP:
            ofs << "push temp " << index << std::endl;
            break;
        default:
            std::cerr 
                << "Error in writePush: "
                << "Unexpected memory segment type. "
                << std::endl;
            std::exit(EXIT_FAILURE);
    }
}

void VMWriter::writePop(const SEGMENT segment, const std::size_t index) {
    switch(segment) {
        case SEGMENT::ARGUMENT:
            ofs << "pop argument " << index << std::endl;
            break;
        case SEGMENT::LOCAL:
            ofs << "pop local " << index << std::endl;
            break;
        case SEGMENT::STATIC:
            ofs << "pop static " << index << std::endl;
            break;
        case SEGMENT::THIS:
            ofs << "pop this " << index << std::endl;
            break;
        case SEGMENT::THAT:
            ofs << "pop that " << index << std::endl;
            break;
        case SEGMENT::POINTER:
            ofs << "pop pointer " << index << std::endl;
            break;
        case SEGMENT::TEMP:
            ofs << "pop temp " << index << std::endl;
            break;
        default:
            std::cerr 
                << "Error in writePop: "
                << "Unexpected memory segment type. "
                << std::endl;
            std::exit(EXIT_FAILURE);
    }
}

void VMWriter::writeArithmetic(const COMMAND command) {
    switch (command) {
        case COMMAND::ADD:
            ofs << "add" << std::endl; break;
        case COMMAND::SUB:
            ofs << "sub" << std::endl; break;
        case COMMAND::NEG:
            ofs << "neg" << std::endl; break;
        case COMMAND::EQ:
            ofs << "eq" << std::endl; break;
        case COMMAND::GT:
            ofs << "gt" << std::endl; break;
        case COMMAND::LT:
            ofs << "lt" << std::endl; break;
        case COMMAND::AND:
            ofs << "and" << std::endl; break;
        case COMMAND::OR:
            ofs << "or" << std::endl; break;
        case COMMAND::NOT:
            ofs << "not" << std::endl; break;
        default:
            std::cerr 
                << "Error in writeArithmetic: "
                << "Unexpected arithmetic command. "
                << std::endl;
            std::exit(EXIT_FAILURE);
    }
}

void VMWriter::writeLabel(const std::string& label) {
    ofs << "label " << label << std::endl;
}

void VMWriter::writeGoto(const std::string& label) {
    ofs << "goto " << label << std::endl;
}

void VMWriter::writeIf(const std::string& label) {
    ofs << "if-goto " << label << std::endl;
}

void VMWriter::writeCall(const std::string& name, std::size_t nArgs) {
    ofs << "call " << name << " " << nArgs << std::endl;
}

void VMWriter::writeFunction(const std::string& name, std::size_t nVars) {
    ofs << "function " << name << " " << nVars << std::endl;
}

void VMWriter::writeReturn() {
    ofs << "return" << std::endl;
}

void VMWriter::close() {
    if (ofs.is_open()) ofs.close();
}