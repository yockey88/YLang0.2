/**
 * \file defines.hpp
 * \author Y
 * \version 0.0.1
 * \brief contains main definitions and other core information
 * \details This file contains the main definitions and other core information as 
 *              well as utility macros and major miscellanous functions.
*/

#ifndef YLANG_DEFINES_HPP
#define YLANG_DEFINES_HPP

#include <string>
#include <cassert>
#include <variant>

#ifdef YLC_DEBUG
#define DEBUG_PRINT(x) std::cout << "< DEBUG ( " << __FILE__ << " : " << __FUNCTION__ << " : " << __LINE__ << " ) > :: " << x << std::endl;
#else
#define DEBUG_PRINT(x) (void)0;
#endif

#ifdef YLC_DEBUG
#define UNIMPLIMENTED std::cout << "\n< Unimplimented Function > :: " << __FUNCTION__ << "\n"; \
                      std::cout << "File :: " << __FILE__ << "\n" << std::endl; \
                      return;


#define UNIMPLIMENTED_DEBUG(x) std::cout << "\n< Unimplimented Function( " << x << " )> :: " << __FUNCTION__ << "\n"; \
                               std::cout << "File :: " << __FILE__ << "\n" << std::endl; \
                               return;

#define UNIMPLIMENTED_RETURN(x) std::cout << "\n< Unimplimented Function > :: " << __FUNCTION__ << "\n"; \
                                std::cout << "File :: " << __FILE__ << "\n" << std::endl; \
                                return x;

#define UNIMPLIMENTED_RETURN_DEBUG(msg , x) std::cout << "\n< Unimplimented Function( " msg " )> :: " << __FUNCTION__ << "\n"; \
                                            std::cout << "File :: " << __FILE__ << "\n" << std::endl; \
                                            return x;
#else
#define UNIMPLIMENTED (void)0;
#define UNIMPLIMENTED_DEBUG(x) (void)0;
#define UNIMPLIMENTED_RETURN(x) (void)0;
#define UNIMPLIMENTED_RETURN_DEBUG(msg , x) (void)0;
#endif

#ifdef YLC_DEBUG
#define ASSERT(x) if ((x)) {} else { std::cout << "\n< Assertion Failed > :: " << #x << "\n"; \
                                     std::cout << "\tFile :: " << __FILE__ << "\n"; \
                                     std::cout << "\tFunction :: " << __FUNCTION__ << "\n"; \
                                     std::cout << "\tLine :: " << __LINE__ << "\n" << std::endl; \
                                     assert(false); }

#define STR(x) #x
#define XSTR(x) STR(x)
#else
#define ASSERT(x) (void)0;
#define PRINT_TOKEN(t) (void)0;
#endif

#if 0
const std::vector<std::string> keywords = {
    "i8" , "i16" , "i32" , "i64" , 
    "u8" , "u16" , "u32" , "u64" ,
    "f32" , "f64" , "char" , "string" , 
    "bool" , 

    "true" , "false" , 
    
    "return" ,

    "export" , "import" , "package" , 

    "enum" , "struct" , "class" , 
    "fn" , 

    "project" , "build" , "space" , "object" ,
    "name" , "version" , "description" , 
    "author" , 

    "root" , "target" , "config" , "type" ,
    "files" , "links" , "dependencies" ,
    
    "exe" , "lib" , 
    "debug" , "release" ,
};

const std::vector<std::string> type_keywords = {

    "i8" , "i16" , "i32" , "i64" , 
    "u8" , "u16" , "u32" , "u64" ,
    "f32" , "f64" , "char" , "string" , 
    "bool" ,
};

const std::vector<std::string> keywords_requires_identifier = {
    "enum" , "struct" , "class" , 
    "fn"
};

const std::vector<std::string> control_keywords = {
    "return" , "import"
};

const std::vector<std::string> obj_file_keywords = {
    "project" , "build" , "space" , "object" ,
    "name" , "version" , "description" , 
    "author" , 

    "root" , "target" , "build" , "config" , "type" ,
    "files" , "links" , "dependencies" ,
    
    "exe" , "library" , 
    "debug" , "release" ,
};

inline bool IsObjectKeyword(const std::string& str) { return (std::find(obj_file_keywords.begin() , obj_file_keywords.end() , str) != obj_file_keywords.end()); }

inline bool IsTypeKeyword(const std::string& str) { return (std::find(type_keywords.begin() , type_keywords.end() , str) != type_keywords.end()); }
inline bool IsControlKeyword(const std::string& str) { return (std::find(control_keywords.begin() , control_keywords.end() , str) != control_keywords.end()); } 

inline bool KeywordRequiresIdentifier(const std::string& keyword) { 
    return (std::find(keywords_requires_identifier.begin() , keywords_requires_identifier.end() , keyword) != keywords_requires_identifier.end());
}
#endif

const static std::string YLANG_VERSION = "0.0.2";

const static std::string TokenTypeStrings[] = {
    "START_OF_FILE" ,
    "KW_I8" , "KW_I16" , "KW_I32" , "KW_I64" ,
    "KW_U8" , "KW_U16" , "KW_U32" , "KW_U64" ,
    "KW_F32" , "KW_F64" , 
    "KW_BOOL" ,
    "KW_CHAR" , "KW_STRING" , "KW_BYTE" ,
    "KW_NULL" , "KW_VOID" , 
    "KW_FN ",
    "KW_IF" , "KW_ELSE" , "KW_FOR" , "KW_WHILE" ,
    "KW_SWITCH" , "KW_RETURN" , "KW_BREAK" , "KW_CONTINUE" ,
    "KW_TRUE" , "KW_FALSE" ,
    "KW_STRUCT" , "KW_CLASS" , "KW_OBJECT" , "KW_ENUM" ,
    "KW_UNION" ,
    "KW_NAMESPACE" ,
    "KW_USING" , "KW_IMPORT" , "KW_EXPORT" , "KW_PUBLIC" ,
    "KW_PRIVATE" , "KW_INCLUDE" ,
    "KW_PRINT" ,
    "IDENTIFIER" ,
    "OP_ADD" , "OP_SUB" , "OP_MUL" , "OP_DIV" , "OP_MOD" ,
    "OP_OPEN_PAREN" , "OP_CLOSE_PAREN" , "OP_OPEN_BRACE" , "OP_CLOSE_BRACE" , "OP_OPEN_BRACKET" , "OP_CLOSE_BRACKET" ,
    "OP_SEMICOLON" , "OP_COMMA" , "OP_DOT" , "OP_COLON" ,
    "OP_DOLLAR" , "OP_HASH" , "OP_AT" , "OP_EXCLAMATION" , "OP_QUESTION" , "OP_AMPERSAND" , "OP_ASSIGN" ,
    "OP_LESS_THAN" , "OP_GREATER_THAN" ,
    "OP_BIT_OR" , "OP_BIT_AND" , "OP_XOR" , "OP_BIT_NOT" ,
    "OP_ADD_ASSIGN" , "OP_SUB_ASSIGN" , "OP_MUL_ASSIGN" , "OP_DIV_ASSIGN" , " OP_MOD_ASSIGN" ,
    "OP_EQUALS" , "OP_NOT_EQUAL" , "OP_LESS_THAN_EQUAL" , "OP_GREATER_THAN_EQUAL" ,
    "OP_LOGICAL_AND" , "OP_LOGICAL_OR" ,
    "OP_BIT_OR_ASSIGN" , "OP_BIT_AND_ASSIGN" , "OP_XOR_ASSIGN" , "OP_BIT_NOT_ASSIGN" ,
    "OP_LEFT_SHIFT" , "OP_RIGHT_SHIFT" , 
    "OP_LEFT_SHIFT_ASSIGN" , "OP_RIGHT_SHIFT_ASSIGN" ,
    "OP_INCREMENT" , "OP_DECREMENT" , "OP_ARROW" , "OP_SCOPE" ,
    "OP_CAPTURE" ,
    "LIT_INT"  , "LIT_HEX" , "LIT_FLOAT" ,
    "LIT_CHAR" , "LIT_STRING" , 
    "COMMENT" ,
    "ERROR_TOKEN" ,
    "END_OF_FILE" 
};

static const std::string VarTypeStrings[] = {
    "I8" , "I16" , "I32" , "I64" ,
    "U8" , "U16" , "U32" , "U64" ,
    "F32" , "F64" ,
    "BOOL" ,
    "CHAR" , "STRING" , "BYTE" ,
    "NULL_T" , "VOID" ,
    "STRUCT" , "CLASS" , "OBJECT" , "ENUM" ,
    "UNION" ,
    "CALLABLE"
};

/// \enum Type
enum class Type {
    START_OF_FILE ,
    
    ////////////////// KEYWORDS //////////////////
    // types
    KW_I8   , KW_I16 , KW_I32 , KW_I64 ,
    KW_U8   , KW_U16 , KW_U32 , KW_U64 ,
    KW_F32  , KW_F64 , 
    KW_BOOL ,
    KW_CHAR , KW_STRING , KW_BYTE ,
    KW_NULL , KW_VOID , // KW_ANY , KW_AUTO ,

    // control flow
    KW_FN ,
    KW_IF , KW_ELSE , KW_FOR , KW_WHILE ,
    KW_SWITCH , KW_RETURN , KW_BREAK , KW_CONTINUE ,
    KW_TRUE , KW_FALSE ,

    // objects
    KW_STRUCT , KW_CLASS , KW_OBJECT , KW_ENUM ,
    KW_UNION ,

    // architecture
    KW_NAMESPACE ,
    KW_USING , KW_IMPORT , KW_EXPORT , KW_PUBLIC ,
    KW_PRIVATE , KW_INCLUDE ,

    // temp
    KW_PRINT ,

    ////////////////// IDENTIFIERS //////////////////
    // type ids
    IDENTIFIER ,

    ////////////////// OPERATORS //////////////////
    // single characters
    OP_ADD , OP_SUB , OP_MUL , OP_DIV , OP_MOD ,
    OP_OPEN_PAREN , OP_CLOSE_PAREN , OP_OPEN_BRACE , OP_CLOSE_BRACE , OP_OPEN_BRACKET , OP_CLOSE_BRACKET ,
    OP_SEMICOLON , OP_COMMA , OP_DOT , OP_COLON ,
    OP_DOLLAR , OP_HASH , OP_AT , OP_EXCLAMATION , OP_QUESTION , OP_AMPERSAND , OP_ASSIGN ,
    OP_LESS_THAN , OP_GREATER_THAN ,
    OP_BIT_OR , OP_BIT_AND , OP_XOR , OP_BIT_NOT ,

    // multi characters
    OP_ADD_ASSIGN , OP_SUB_ASSIGN , OP_MUL_ASSIGN , OP_DIV_ASSIGN , OP_MOD_ASSIGN ,
    OP_EQUALS , OP_NOT_EQUAL , OP_LESS_THAN_EQUAL , OP_GREATER_THAN_EQUAL ,
    OP_LOGICAL_AND , OP_LOGICAL_OR ,
    OP_BIT_OR_ASSIGN , OP_BIT_AND_ASSIGN , OP_XOR_ASSIGN , OP_BIT_NOT_ASSIGN ,
    OP_LEFT_SHIFT , OP_RIGHT_SHIFT , 
    OP_LEFT_SHIFT_ASSIGN , OP_RIGHT_SHIFT_ASSIGN ,
    OP_INCREMENT , OP_DECREMENT , OP_ARROW , OP_SCOPE ,
    OP_CAPTURE ,

    ////////////////// LITERALS //////////////////
    LIT_INT  , LIT_HEX , LIT_FLOAT ,
    LIT_CHAR , LIT_STRING , 

    COMMENT ,

    ERROR_TOKEN ,
    END_OF_FILE
};

enum class VarType : uint8_t {
//  0      1     2     3 
    I8   , I16 , I32 , I64 ,
//  4     5      6     7
    U8   , U16 , U32 , U64 ,
//  8      9
    F32  , F64 , 
//  10    
    BOOL ,
//  11     12       13
    CHAR , STRING , BYTE ,
//  14      15
    NULL_T , VOID ,
//  16
    STRUCT , CLASS , OBJECT , ENUM ,
//  17
    UNION ,
// 18
    CALLABLE
};

namespace ylang {

    /// \struct Token
    /// \details This is the token, which contains all the information about a token
    ///          after it is converted from a RawToken object.
    struct Token {
        Type type;

        uint32_t line;
        uint32_t column;
        std::string token;

        Token(Type type , uint32_t line , uint32_t column , std::string token)
            : type(type) , line(line) , column(column) , token(token) {}
    };

    
    using VarValue = std::variant<int8_t , int16_t , int32_t , int64_t ,
                                  uint8_t , uint16_t , uint32_t , uint64_t ,                           
                                  float , double , bool , char , std::string>;

    VarValue operator+(const VarValue& lhs , const VarValue& rhs);
    VarValue operator-(const VarValue& lhs , const VarValue& rhs);
    VarValue operator*(const VarValue& lhs , const VarValue& rhs);
    VarValue operator/(const VarValue& lhs , const VarValue& rhs);
    VarValue operator%(const VarValue& lhs , const VarValue& rhs);
    bool operator==(const VarValue& lhs , const VarValue& rhs);
    bool operator!=(const VarValue& lhs , const VarValue& rhs);
    bool operator<(const VarValue& lhs , const VarValue& rhs);
    bool operator<=(const VarValue& lhs , const VarValue& rhs);
    bool operator>(const VarValue& lhs , const VarValue& rhs);
    bool operator>=(const VarValue& lhs , const VarValue& rhs);
    void operator<<(std::ostream& os , const VarValue& value);

    VarType VarTypeFromToken(Type type);

    struct Variable {
        VarType type;
        VarValue value;
        std::string name;
    };

    void operator<<(std::ostream& os , const Variable& var);

}

#endif // !YLANG_DEFINES_HPP