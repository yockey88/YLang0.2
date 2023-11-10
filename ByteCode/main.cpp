#include <iostream>
#include <cassert>

#include "compiler.hpp"

int main() {
    ylang::InitVM();

    ylang::Chunk chunk;

    uint32_t constant = ylang::AddConstant(chunk , 1.2);
    ylang::WriteChunk(chunk , ylang::OpCode::OP_CONSTANT , 123);
    ylang::WriteChunk(chunk , constant , 123);

    constant = ylang::AddConstant(chunk , 3.4);
    ylang::WriteChunk(chunk , ylang::OpCode::OP_CONSTANT , 123);
    ylang::WriteChunk(chunk , constant , 123);

    ylang::WriteChunk(chunk , ylang::OpCode::OP_ADD , 123);

    constant = ylang::AddConstant(chunk , 5.6);
    ylang::WriteChunk(chunk , ylang::OpCode::OP_CONSTANT , 123);
    ylang::WriteChunk(chunk , constant , 123);

    ylang::WriteChunk(chunk , ylang::OpCode::OP_DIVIDE , 123);
    ylang::WriteChunk(chunk , ylang::OpCode::OP_NEGATE , 123);
    
    ylang::WriteChunk(chunk , ylang::OpCode::OP_RETURN , 123);
    
    ylang::Interpret(&chunk);
    ylang::FreeVM();
    ylang::FreeChunk(chunk);
    return 0;
}