#include "lexer.hpp"

#include <iostream>

#include "defines.hpp"
#include "util.hpp"

namespace ylang {
    
    void Lexer::InitTokenTypeMaps() {
        keyword_map["i8"] = Type::KW_I8;
        keyword_map["i16"] = Type::KW_I16;
        keyword_map["i32"] = Type::KW_I32;
        keyword_map["i64"] = Type::KW_I64;

        keyword_map["u8"] = Type::KW_U8;
        keyword_map["u16"] = Type::KW_U16;
        keyword_map["u32"] = Type::KW_U32;
        keyword_map["u64"] = Type::KW_U64;

        keyword_map["f32"] = Type::KW_F32;
        keyword_map["f64"] = Type::KW_F64;

        keyword_map["bool"] = Type::KW_BOOL;

        keyword_map["char"] = Type::KW_CHAR;
        keyword_map["string"] = Type::KW_STRING;
        keyword_map["byte"] = Type::KW_BYTE;

        keyword_map["null"] = Type::KW_NULL;
        keyword_map["void"] = Type::KW_VOID;

        keyword_map["fn"] = Type::KW_FN;
        keyword_map["if"] = Type::KW_IF;
        keyword_map["else"] = Type::KW_ELSE;
        keyword_map["for"] = Type::KW_FOR;
        keyword_map["while"] = Type::KW_WHILE;
        keyword_map["switch"] = Type::KW_SWITCH;
        keyword_map["return"] = Type::KW_RETURN;
        keyword_map["break"] = Type::KW_BREAK;
        keyword_map["continue"] = Type::KW_CONTINUE;
        keyword_map["true"] = Type::KW_TRUE;
        keyword_map["false"] = Type::KW_FALSE;

        keyword_map["struct"] = Type::KW_STRUCT;
        keyword_map["class"] = Type::KW_CLASS;
        keyword_map["object"] = Type::KW_OBJECT;
        keyword_map["enum"] = Type::KW_ENUM;
        keyword_map["union"] = Type::KW_UNION;

        keyword_map["namespace"] = Type::KW_NAMESPACE;
        keyword_map["using"] = Type::KW_USING;
        keyword_map["import"] = Type::KW_IMPORT;
        keyword_map["export"] = Type::KW_EXPORT;
        keyword_map["public"] = Type::KW_PUBLIC;
        keyword_map["private"] = Type::KW_PRIVATE;
        keyword_map["include"] = Type::KW_INCLUDE;

        keyword_map["print"] = Type::KW_PRINT;
    }

    void Lexer::HandleWhitespace() {
        while (IsWhitespace(Peek())) {
            if (Peek() == '\n') {
                NewLine();
            } else {
                Consume();
            }

            if (IsEOF()) break;
        }
    }
    
    void Lexer::HandleOperator() {
        if (Peek() == '\'') {
            HandleChar();
            return;
        }

        if (Peek() == '"') {
            HandleString();
            return;
        }

        char c = Peek();
        Advance();
        switch (c) {
            case '(':AddToken(Type::OP_OPEN_PAREN); break;
            case ')': AddToken(Type::OP_CLOSE_PAREN); break;
            case '{': AddToken(Type::OP_OPEN_BRACE); break;
            case '}': AddToken(Type::OP_CLOSE_BRACE); break;
            case '[': AddToken(Type::OP_OPEN_BRACKET); break;
            case ']': AddToken(Type::OP_CLOSE_BRACKET); break;
            case ';': AddToken(Type::OP_SEMICOLON); break;
            case ',': AddToken(Type::OP_COMMA); break;
            case '.': AddToken(Type::OP_DOT); break;
            case '#': AddToken(Type::OP_HASH); break;
            case '@': AddToken(Type::OP_AT); break;
            case '?': AddToken(Type::OP_QUESTION); break;
            case '!': AddToken(Type::OP_EXCLAMATION); break;
            case '*': AddToken(Match('=') ? Type::OP_MUL_ASSIGN : Type::OP_MUL); break;
            case '%': AddToken(Match('=') ? Type::OP_MOD_ASSIGN : Type::OP_MOD); break;
            case '^': AddToken(Match('=') ? Type::OP_XOR_ASSIGN : Type::OP_XOR); break;
            case '~': AddToken(Match('=') ? Type::OP_BIT_NOT_ASSIGN : Type::OP_BIT_NOT); break;
            case ':': AddToken(Match(':') ? Type::OP_SCOPE : Type::OP_COLON); break;
            // Maybe for unwrapping YLang objects (T ${obj}> foo runtime equivalent of obj<T> foo or something like that)
            case '$': AddToken(Match('>') ? Type::OP_CAPTURE : Type::OP_DOLLAR); break; 
            case '|': 
                if (Check('|')) {
                    Advance();
                    AddToken(Type::OP_LOGICAL_OR);
                } else if (Check('=')) {
                    Advance();
                    AddToken(Type::OP_BIT_OR_ASSIGN);
                } else {
                    AddToken(Type::OP_BIT_OR);
                }
            break;
            case '&':
                if (Check('&')) {
                    Advance();
                    AddToken(Type::OP_LOGICAL_AND);
                } else if (Check('=')) {
                    Advance();
                    AddToken(Type::OP_BIT_AND_ASSIGN);
                } else {
                    AddToken(Type::OP_BIT_AND);
                }
            break;
            case '<': 
                if (Check('<')) {
                    Advance();
                    AddToken(Match('=') ? Type::OP_LEFT_SHIFT_ASSIGN : Type::OP_LEFT_SHIFT);
                } else if (Check('=')) {
                    Advance();
                    AddToken(Type::OP_LESS_THAN_EQUAL);
                } else {
                    AddToken(Type::OP_LESS_THAN);
                }
            break;
            case '>': 
                if (Check('>')) {
                    Advance();
                    AddToken(Match('=') ? Type::OP_RIGHT_SHIFT_ASSIGN : Type::OP_RIGHT_SHIFT);
                } else if (Check('=')) {
                    Advance();
                    AddToken(Type::OP_GREATER_THAN_EQUAL);
                } else {
                    AddToken(Type::OP_GREATER_THAN);
                }
            break;
            case '+':
                if (Check('+')) {
                    Advance();
                    AddToken(Type::OP_INCREMENT);
                } else if (Check('=')) {
                    Advance();
                    AddToken(Type::OP_ADD_ASSIGN);
                } else {
                    AddToken(Type::OP_ADD);
                }
            break;
            case '-':
                if (Check('-')) {
                    Advance();
                    AddToken(Type::OP_DECREMENT);
                } else if (Check('>')) {
                    Advance();
                    AddToken(Type::OP_ARROW);
                } else if (Check('=')) {
                    Advance();
                    AddToken(Type::OP_SUB_ASSIGN);
                } else {
                    AddToken(Type::OP_SUB);
                }
            break;
            case '=': 
                if (Check('=')) {
                    Advance();
                    AddToken(Type::OP_EQUALS);
                } else {
                    AddToken(Type::OP_ASSIGN);
                }
            break;
            case '/': 
                if (Check('/') || Check('*')) {
                    HandleComment();
                } else if (Check('=')) {
                    Advance();
                    AddToken(Type::OP_DIV_ASSIGN);
                } else {
                    AddToken(Type::OP_DIV);
                }
            break;
            default: break;
        } 

    }

    /// \todo Handle comments
    void Lexer::HandleComment() {

        if (Peek() == '/') {
            while (Peek() != '\n' && !IsEOF())
                Advance();
        } else if (Peek() == '*') {
            Advance();
            while (!IsEOF()) {
                if (Peek() == '\n') {
                    Advance();
                    NewLine(false);
                    continue;
                }
                if (Peek() == '*' && PeekNext() == '/') break;
                Advance();
            } 

            if (Peek() != '*')
                throw lexer_error(ErrorLevel::FATAL , line , column , "Unterminated block comment");
            Advance();

            if (Peek() != '/')
                throw lexer_error(ErrorLevel::FATAL , line , column , "Undefined behavior");
            Advance();
        }
        
        DiscardToken();
        // AddToken(Type::COMMENT);
    }

    void Lexer::HandleChar() {
        Consume();
        Advance();
        AddToken(Type::LIT_CHAR);

        if (Peek() != '\'')
            throw lexer_error(ErrorLevel::FATAL , line , column , "Unterminated character literal");
        Consume();
    }

    /// \todo Handle string literals
    void Lexer::HandleString() {
        int start_line = line;
        int start_column = column;
        Consume();
        while (Peek() != '"' && !IsEOF())
            Advance();
        AddToken(Type::LIT_STRING);
        
        if (Peek() != '"')
            throw lexer_error(ErrorLevel::FATAL , start_line , start_column , "Unterminated string literal");
        Consume();
    }

    void Lexer::HandleNumeric() {
        while (IsNumeric(Peek())) {
            Advance();
            if (IsEOF()) break;
        }

        if (Peek() == '.' && IsNumeric(PeekNext())) {
            Advance();
            while (IsNumeric(Peek())) {
                Advance();
                if (IsEOF()) break;
            }
            AddToken(Type::LIT_FLOAT);
        } else if (Peek() == 'x' && IsNumeric(PeekNext())) {
            Advance();
            while (IsNumeric(Peek())) {
                Advance();
                if (IsEOF()) break;
            }
            AddToken(Type::LIT_HEX);
        } else {
            AddToken(Type::LIT_INT);
        }
    }

    void Lexer::HandleAlphaNumeric() {
        while (IsAlphaNumeric(Peek())) {
            Advance();
            if (IsEOF()) break;
        }

        if (IsKeyword(curr_token)) {
            AddToken(GetTypeForKeyword(curr_token));
        } else {
            AddToken(Type::IDENTIFIER);
        }
    }

    bool Lexer::Lex() {
        if (this->error_handler == nullptr)
            throw lexer_error(ErrorLevel::FATAL , 0 , 0 , "No error handler found");

        if (this->source == "<n>" || this->source.length() == 0)
            throw lexer_error(ErrorLevel::FATAL , 0 , 0 , "No source code found");

        tokens.push_back(Token(Type::START_OF_FILE, 0 , 0 , Util::GetCurrFileName()));
        while (src_index < src_length) {
            try {
                if (IsWhitespace(Peek())) {
                    HandleWhitespace();
                    continue;
                }

                if (IsOperator(Peek())) {
                    HandleOperator();
                    continue;
                }

                if (IsNumeric(Peek())) {
                    HandleNumeric();
                    continue;
                }

                if (IsAlpha(Peek()) || Peek() == '_') {
                    HandleAlphaNumeric();
                    continue;
                }
            } catch (const lexer_error& e) {
                error_handler->HandleLexerError(e , source , start_of_line_index , src_index);
                return false;
            }
        }
        tokens.push_back(Token(Type::END_OF_FILE , 0 , 0 , Util::GetCurrFileName()));

        return true;
    }

}