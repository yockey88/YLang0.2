/**
 * \file lexer.hpp
 * \author Y
 * \version 0.0.2
 * \brief Lexer class header file, defines the interface for the Lexer 
*/

#ifndef YLANG_LEXER_HPP
#define YLANG_LEXER_HPP

#include <string>
#include <vector>
#include <map>

#include "defines.hpp"
#include "errors.hpp"

/// \namespace ylang namespace
/// \brief contains all the ylang compiler related code
namespace ylang {

    /// \class Lexer
    /// \brief used internally by the ylang compiler to perform lexical
    ///         analysis on the source code
    /// \note this class is not meant to be used by the end user
    class Lexer {

        const std::vector<char> operators = {
            '+' , '-' , '*' , '/' , '%' ,
            '(' , ')' , '{' , '}' , '[' , ']' ,
            ';' , ',' , '.' , '"' , '\'' , ':' ,
            '$' , '#' , '@' , '!' , '?' , '&' ,
            '|' , '^' , '~' , '=' , '<' , '>'
        };

        const std::vector<std::string> keywords = {
            // types
            "i8"      , "i16"     , "i32"    , "i64"      ,
            "u8"      , "u16"     , "u32"    , "u64"      ,
            "f32"     , "f64"     , "double" , "bool"     , 
            "char"    , "string"  , "byte"   ,
            "null"   , "void"     , // "any"    , "auto"     ,

            // control flow
            "fn"      ,
            "if"      , "else"    , "for"    , "while"    ,
            "switch"  , "return"  , "break"  , "continue" , 
            "true"    , "false"   , 

            // objects
            "struct"  , "class"   , "object" , "enum"     ,
            "union"   ,
            
            // architecture
            "namespace",
            "using"   , "import"  , "export" , "public"   ,
            "private" , "include" ,

            // temp
            "print"
        };

        std::map<std::string , Type> keyword_map;
        std::map<std::string , Type> identifier_map;

        std::shared_ptr<ErrorHandler> error_handler = nullptr;
        uint32_t start_of_line_index = 0;
        uint32_t src_index = 0;
        uint32_t src_length = 0;
        uint32_t line = 1;
        uint32_t column = 1;

        std::string curr_token = "";
        std::string source = "<n>";

        std::vector<Token> tokens{};

        inline bool IsNumeric(char c) { return (c >= '0' && c <= '9'); }

        inline bool IsAlpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }

        inline bool IsAlphaNumeric(char c) { return (IsAlpha(c) || IsNumeric(c) || c == '_'); }

        inline bool IsOperator(char c) const {
            return (std::find(operators.begin() , operators.end() , c) != operators.end());
        } 

        inline bool IsWhitespace(char c) {
            return (c == ' ') || (c == '\t') || (c == '\f') || (c == '\v') || (c == '\n');
        }

        inline bool IsKeyword(const std::string& str) const {
            return (std::find(keywords.begin() , keywords.end() , str) != keywords.end());
        }

        inline Type GetTypeForKeyword(const std::string& str) const { return keyword_map.at(str); }
        inline Type GetTypeForIdentifier(const std::string& str) const { return identifier_map.at(str); }

        /// \brief iterates src if flag is set, infrements line and sets column to 1, also sets error helper
        ///         variables to the correct values
        /// \param iterate_src_index (optional, default = true)  
        inline void NewLine(bool iterate_src_index = true) {
            if (iterate_src_index)
                ++src_index;
            ++line;
            start_of_line_index = src_index;
            column = 1;
        }

        /// \brief increments to next character without saving to current token
        inline void Consume() {
            ++column;
            ++src_index;
        }

        /// \brief saves to current token and increments to next character
        inline void Advance() {
            curr_token += source[src_index];
            Consume();
        }

        inline void DiscardToken() { curr_token = ""; }

        inline void AddToken(Type type) {
            tokens.push_back(Token(type , line , column , curr_token));
            curr_token = "";
        }

        /// \brief looks at current character while also checking bounds 
        inline char Peek() const {
            if (IsEOF())
                return '\0';
            return source[src_index];
        }

        /// \brief looks at next character while also checking bounds
        inline char PeekNext() const {
            if (src_index + 1 >= src_length)
                return '\0';
            return source[src_index + 1];
        }

        inline bool Check(char c) const {
            if (IsEOF()) return false;
            return (source[src_index] == c);
        }

        inline bool Match(char c) {
            if (!Check(c)) return false;

            Advance();
            return true;
        }

        /// \brief fills map with strings corresponfing ylang::Type to identify token types 
        void InitTokenTypeMaps();

        void HandleWhitespace();
        void HandleComment();
        void HandleChar();
        void HandleString();
        void HandleNumeric();
        void HandleAlphaNumeric();
        void HandleOperator();

        inline bool IsEOF() const { return (src_index >= src_length); }

        public:

            /// \param err_handler pointer to the error handler
            /// \param src source code to lex
            Lexer(std::shared_ptr<ErrorHandler> err_handler , const std::string& src)
                : error_handler(err_handler) , source(src) , src_length(static_cast<uint32_t>(src.length())) {
                InitTokenTypeMaps();
            }
            
            ~Lexer() {}

            /// \brief Method called to lex the source code
            /// \throw ylang_lexer_error if unhandleable error is encountered
            bool Lex();

            /// \return std::vector<Token> vector of tokens
            inline const std::vector<Token> GetTokens() const { return tokens; }

    };

}

#endif // !YLANG_LEXER_HPP