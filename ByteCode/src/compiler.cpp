#include "compiler.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include <spdlog/fmt/fmt.h>

namespace ylang {

    VM vm;
    Scanner scanner;

namespace util {
    
    std::string ReadFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << path << "\n";
            return "";
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

} // namespace util
    
    void PrintValue(const Value& value) {
        std::cout << fmt::format("{:g}" , value);
    }

    void WriteChunk(Chunk& chunk , uint8_t byte , uint32_t line) {
        chunk.code.push_back(byte);
        chunk.lines.push_back(line);
    }

    uint32_t AddConstant(Chunk& chunk , Value value) {
        chunk.constants.push_back(value);
        return chunk.constants.size() - 1;
    }

    void DisassembleChunk(const Chunk& chunk , const std::string& name) {
        std::cout << "== " << name << " ==\n";
        for (uint32_t i = 0; i < chunk.code.size();)
            i = DisassembleInstruction(chunk , i);
    }
    
    void FreeChunk(Chunk& chunk) {
        chunk.constants.clear();
        chunk.code.clear();
        chunk.lines.clear();
    }
    
    uint32_t DisassembleInstruction(const Chunk& chunk , size_t offset) {
        std::cout << fmt::format("{:#04x} " , offset);

        if (offset > 0 && chunk.lines[offset] == chunk.lines[offset - 1])
            std::cout << "   | ";
        else
            std::cout << fmt::format("{:4d} " , chunk.lines[offset]);

        uint8_t instruction = chunk.code[offset];
        switch (instruction) {
            case OpCode::OP_CONSTANT: return ConstantInstruction("OP_CONSTANT" , chunk , offset);
            case OpCode::OP_ADD:      return SimpleInstruction("OP_ADD" , offset);
            case OpCode::OP_SUBTRACT: return SimpleInstruction("OP_SUBTRACT" , offset);
            case OpCode::OP_MULTIPLY: return SimpleInstruction("OP_MULTIPLY" , offset);
            case OpCode::OP_DIVIDE:   return SimpleInstruction("OP_DIVIDE" , offset);
            case OpCode::OP_NEGATE:   return SimpleInstruction("OP_NEGATE" , offset);
            case OpCode::OP_RETURN:   return SimpleInstruction("OP_RETURN" , offset);
            default: 
                std::cout << "<!>[Unknown opcode] :: " << instruction << "\n"; 
                return offset + 1;
        }
    }
    
    uint32_t SimpleInstruction(const std::string& name , uint32_t offset) {
        std::cout << name << "\n";
        return offset + 1;
    }
    
    uint32_t ConstantInstruction(const std::string& name , const Chunk& chunk , uint32_t offset) {
        uint8_t constant = chunk.code[offset + 1];
        std::cout << fmt::format("{:<16} {:<4} " , name , constant);
        PrintValue(chunk.constants[constant]);
        std::cout << "\n";
        return offset + 2;
    }
    
    Token ScanToken() {
        SkipWhitespace();
        scanner.start = scanner.current;

        if (IsAtEnd()) return MakeToken(TokenType::END_OF_FILE);

        char c = Advance();

        if (IsAlpha(c)) return Identifier();
        if (IsDigit(c)) return Number();

        switch (c) {
            case '(': return MakeToken(TokenType::TOKEN_LEFT_PAREN);
            case ')': return MakeToken(TokenType::TOKEN_RIGHT_PAREN);
            case '{': return MakeToken(TokenType::TOKEN_LEFT_BRACE);
            case '}': return MakeToken(TokenType::TOKEN_RIGHT_BRACE);
            case ';': return MakeToken(TokenType::TOKEN_SEMICOLON);
            case ',': return MakeToken(TokenType::TOKEN_COMMA);
            case '.': return MakeToken(TokenType::TOKEN_DOT);
            case '-': return MakeToken(TokenType::TOKEN_MINUS);
            case '+': return MakeToken(TokenType::TOKEN_PLUS);
            case '/': return MakeToken(TokenType::TOKEN_SLASH);
            case '*': return MakeToken(TokenType::TOKEN_STAR);
            case '!':
                return MakeToken(
                    Match('=') ? TokenType::TOKEN_BANG_EQUAL : TokenType::TOKEN_BANG);
            case '=':
                return MakeToken(
                    Match('=') ? TokenType::TOKEN_EQUAL_EQUAL : TokenType::TOKEN_EQUAL);
            case '<':
                return MakeToken(
                    Match('=') ? TokenType::TOKEN_LESS_EQUAL : TokenType::TOKEN_LESS);
            case '>':
                return MakeToken(
                    Match('=') ? TokenType::TOKEN_GREATER_EQUAL : TokenType::TOKEN_GREATER);
            case '"': return String();
        }

        return ErrorToken("Unexpected character.");
    }

    Token MakeToken(TokenType type) {
        Token token;
        token.type = type;
        token.start = scanner.start;
        token.length = (uint32_t)(scanner.current - scanner.start);
        token.line = scanner.line;
        return token;
    }
    
    Token ErrorToken(const std::string& msg) {
        Token token;
        token.type = TokenType::ERROR_TOKEN;
        token.start = msg.c_str();
        token.length = (uint32_t)msg.size();
        token.line = scanner.line;
        return token;
    }

    Token String() {
        while (Peek() != '"' && !IsAtEnd()) {
            if (Peek() == '\n') ++scanner.line;
            Advance();
        }

        if (IsAtEnd()) return ErrorToken("Unterminated string.");

        Advance();
        return MakeToken(TokenType::TOKEN_STRING);
    }
    
    Token Identifier() {
        while (IsAlpha(Peek()) || IsDigit(Peek())) Advance();

        return MakeToken(IdentifierType());
    }

    TokenType IdentifierType() {
        switch (scanner.start[0]) {
            case 'a': return CheckKeyword(1 , 2 , "nd" , TokenType::TOKEN_AND);
            case 'c': return CheckKeyword(1 , 4 , "lass" , TokenType::TOKEN_CLASS);
            case 'e': return CheckKeyword(1 , 3 , "lse" , TokenType::TOKEN_ELSE);
            case 'f':
                if (scanner.current - scanner.start > 1) {
                    switch (scanner.start[1]) {
                        case 'a': return CheckKeyword(2 , 3 , "lse" , TokenType::TOKEN_FALSE);
                        case 'o': return CheckKeyword(2 , 1 , "r" , TokenType::TOKEN_FOR);
                        case 'u': return CheckKeyword(2 , 1 , "n" , TokenType::TOKEN_FUN);
                    }
                }
            break;            
            case 'i': return CheckKeyword(1 , 1 , "f" , TokenType::TOKEN_IF);
            case 'n': return CheckKeyword(1 , 2 , "il" , TokenType::TOKEN_NIL);
            case 'o': return CheckKeyword(1 , 1 , "r" , TokenType::TOKEN_OR);
            case 'p': return CheckKeyword(1 , 4 , "rint" , TokenType::TOKEN_PRINT);
            case 'r': return CheckKeyword(1 , 5 , "eturn" , TokenType::TOKEN_RETURN);
            case 's': return CheckKeyword(1 , 4 , "uper" , TokenType::TOKEN_SUPER);
            case 't':
                if (scanner.current - scanner.start > 1) {
                    switch (scanner.start[1]) {
                        case 'h': return CheckKeyword(2 , 2 , "is" , TokenType::TOKEN_THIS);
                        case 'r': return CheckKeyword(2 , 2 , "ue" , TokenType::TOKEN_TRUE);
                    }
                }
            break;
            case 'v': return CheckKeyword(1 , 2 , "ar" , TokenType::TOKEN_VAR);
            case 'w': return CheckKeyword(1 , 4 , "hile" , TokenType::TOKEN_WHILE);
        }
        return TokenType::TOKEN_IDENTIFIER;
    }
    
    TokenType CheckKeyword(uint32_t start , uint32_t length , const char* rest , TokenType type) {
        if (scanner.current - scanner.start == start + length &&
            memcmp(scanner.start + start , rest , length) == 0) {
            return type;
        }
        return TokenType::TOKEN_IDENTIFIER;
    }

    Token Number() {
        while (IsDigit(Peek())) Advance();

        if (Peek() == '.' && IsDigit(PeekNext())) {
            Advance();
            while (IsDigit(Peek())) Advance();
        }

        return MakeToken(TokenType::TOKEN_NUMBER);
    }

    void SkipWhitespace() {
        for (;;) {
            char c = Peek();
            switch (c) {
                case ' ':
                case '\r':
                case '\t':
                    Advance();
                    break;
                case '\n':
                    ++scanner.line;
                    Advance();
                    break;
                case '/':
                    if (PeekNext() == '/') {
                        while (Peek() != '\n' && !IsAtEnd()) Advance();
                    } else {
                        return;
                    }
                    break;
                default:
                    return;
            }
        }
    }
    
    bool IsAlpha(char c) {
        return ((c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                 c == '_');
    }

    bool IsDigit(char c) {
        return (c >= '0' && c <= '9');
    }

    char Peek() {
        return *scanner.current;
    }
    
    char PeekNext() {
        if (IsAtEnd()) return '\0';
        return scanner.current[1];
    }

    bool IsAtEnd() {
        return *scanner.current == '\0';
    }

    void Consume(TokenType type , const std::string& msg) {
        throw std::runtime_error("Not implemented");
    }
    
    char Advance() {
        ++scanner.current;
        return scanner.current[-1];
    }
    
    bool Match(char expected) {
        if (IsAtEnd()) return false;
        if (*scanner.current != expected) return false;

        ++scanner.current;
        return true;
    }

    void InitScanner(const std::string& source) {
        scanner.start = source.c_str();
        scanner.current = source.c_str();
        scanner.line = 1;
    }

    void InitVM() {
        ResetStack();
    }

    bool Compile(const std::string& source , Chunk& chunk) {
        InitScanner(source);
        AdvanceCompiler();
        Expression();
        Consume(TokenType::END_OF_FILE , "Expect end of expression.");
        return true;
    }
    
    void AdvanceCompiler() {
        
    }

    void Expression() {

    }

    InterpretResult Interpret(const std::string& source) {
        Chunk chunk;

        if (!Compile(source , chunk)) {
            FreeChunk(chunk);
            return InterpretResult::INTERPRET_COMPILE_ERROR;
        }

        vm.chunk = &chunk;
        vm.ip = vm.chunk->code.data();

        InterpretResult result = Run();

        FreeChunk(chunk);
        return result;
    }

    void FreeVM() {

    }
    
    InterpretResult Run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants[READ_BYTE()])
#define BINARY_OP(op)         \
        do {                  \
            Value b = Pop();  \
            Value a = Pop();  \
            Push(a op b);     \
        } while (false)

        for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
            printf("          ");
            for (Value* slot = vm.stack; slot < vm.stack_top; ++slot) {
                printf("[ ");
                PrintValue(*slot);
                printf(" ]");
            }
            printf("\n");
            DisassembleInstruction(*vm.chunk , (uint32_t)(vm.ip - vm.chunk->code.data()));
#endif
            uint8_t instruction;
            switch (instruction = READ_BYTE()) {
                case OpCode::OP_CONSTANT: {
                    Value value = READ_CONSTANT();
                    Push(value);
                } break;
                case OpCode::OP_ADD: BINARY_OP(+); break;
                case OpCode::OP_SUBTRACT: BINARY_OP(-); break;
                case OpCode::OP_MULTIPLY: BINARY_OP(*); break;
                case OpCode::OP_DIVIDE: BINARY_OP(/); break;
                case OpCode::OP_NEGATE: Push(-Pop()); break;
                case OpCode::OP_RETURN: {
                    PrintValue(Pop());
                    printf("\n");
                    return InterpretResult::INTERPRET_OK;
                }
            }
        }

#undef BINARY_OP
#undef READ_CONSTANT
#undef READ_BYTE
    }

    InterpretResult Interpret(Chunk* chunk) {
        vm.chunk = chunk;
        vm.ip = vm.chunk->code.data();
        return Run();
    }

    void ResetStack() {
        vm.stack_top = vm.stack;
    }
    
    void Push(Value value) {
        *vm.stack_top = value;
        ++vm.stack_top;
    }
    
    Value Pop() {
        --vm.stack_top;
        return *vm.stack_top;
    }

} // namespace ylang