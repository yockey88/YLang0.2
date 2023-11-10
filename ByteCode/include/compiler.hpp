#ifndef YL_COMPILER_HPP
#define YL_COMPILER_HPP

#include <cstdint>
#include <vector>
#include <string>

#define DEBUG_TRACE_EXECUTION
#define STACK_MAX 256

namespace ylang {
    
    typedef double Value;

namespace util {
    
    template <typename T>
    uint32_t ExtendArr(uint32_t old_cap , T *& arr) {
        uint32_t new_cap = old_cap < 8 ? 8 : old_cap * 2;
        T* new_arr = new T[new_cap];
        for (uint32_t i = 0; i < old_cap; ++i)
            new_arr[i] = arr[i];
        delete[] arr;
        arr = new_arr;

        return new_cap;
    }

    std::string ReadFile(const std::string& path);

}; // namespace util

    enum TokenType {
        // Single-character tokens.
        TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
        TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
        TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
        TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,
        // One or two character tokens.
        TOKEN_BANG, TOKEN_BANG_EQUAL,
        TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
        TOKEN_GREATER, TOKEN_GREATER_EQUAL,
        TOKEN_LESS, TOKEN_LESS_EQUAL,
        // Literals.
        TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,
        // Keywords.
        TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
        TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
        TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
        TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,


        ERROR_TOKEN , END_OF_FILE
    };

    enum OpCode : uint8_t {
        OP_CONSTANT ,
        OP_ADD ,
        OP_SUBTRACT ,
        OP_MULTIPLY ,
        OP_DIVIDE ,
        OP_NEGATE ,
        OP_RETURN 
    };

    enum class InterpretResult {
        INTERPRET_OK ,
        INTERPRET_COMPILE_ERROR ,
        INTERPRET_RUNTIME_ERROR
    };

    struct Chunk {
        std::vector<uint8_t> code{};
        std::vector<uint32_t> lines{};
        std::vector<Value> constants{};
    };

    struct Token {
        TokenType type = TokenType::ERROR_TOKEN;
        const char* start = nullptr;
        uint32_t length = 0;
        uint32_t line = 0;
    };

    struct Scanner {
        const char* start = nullptr;
        const char* current = nullptr;
        uint32_t index = 0;
        uint32_t line = 1;
    };

    struct Parser {
        Token current{};
        Token previous{};
    };

    struct VM {
        Chunk* chunk = nullptr;
        uint8_t* ip = nullptr;
        Value stack[STACK_MAX];
        Value* stack_top = nullptr;
    };

    void PrintValue(const Value& value);

    void WriteChunk(Chunk& chunk , uint8_t byte , uint32_t line);
    uint32_t AddConstant(Chunk& chunk , Value value);
    void FreeChunk(Chunk& chunk);

    void DisassembleChunk(const Chunk& chunk , const std::string& name);
    uint32_t DisassembleInstruction(const Chunk& chunk , size_t offset);
    uint32_t SimpleInstruction(const std::string& name , uint32_t offset);
    uint32_t ConstantInstruction(const std::string& name , const Chunk& chunk , uint32_t offset);

    Token ScanToken();
    Token MakeToken(TokenType type);
    static Token ErrorToken(const std::string& msg);
    static Token String();
    static Token Identifier();
    static TokenType IdentifierType();
    static TokenType CheckKeyword(uint32_t start , uint32_t length , const char* rest , TokenType type);
    static Token Number();
    void SkipWhitespace();
    static bool IsAlpha(char c);
    static bool IsDigit(char c);
    static char Peek();
    static char PeekNext();
    static bool IsAtEnd();
    static void Consume(TokenType type , const std::string& msg);
    static char Advance();
    static bool Match(char expected);
    void InitScanner(const std::string& source);

    static void AdvanceParser();

    void InitVM();
    bool Compile(const std::string& source , Chunk& chunk);
    void AdvanceCompiler();
    void Expression();
    InterpretResult Interpret(const std::string& source);
    void FreeVM();
    InterpretResult Run();
    InterpretResult Interpret(Chunk* chunk);
    void ResetStack();
    void Push(Value value);
    Value Pop();

} // namespace ylang

#endif // !YL_COMPILER_HPP
