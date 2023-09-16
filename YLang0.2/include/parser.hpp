/**
 * \file parser.hpp
 * \version 0.0.1
 * \author Y
 * \brief Parser class header file, defines the interface for the Parser
*/

#ifndef YL_PARSER_HPP
#define YL_PARSER_HPP

#include <vector>
#include <memory>

#include "defines.hpp"
#include "errors.hpp"
#include "ast.hpp"

namespace ylang {

    class Parser {

        std::shared_ptr<ErrorHandler> err_handler = nullptr;

        Type var_type = Type::ERROR_TOKEN;
        bool in_var_decl = false;

        uint32_t index = 0;
        bool abort_parse = false;

        std::vector<Token> lexemes{};
        std::vector<std::shared_ptr<ASTStmnt>> statements{};

        inline parser_error GetError(ErrorLevel level , const std::string& msg) {
            err_handler->SubmitError(Error(level , Peek().line , Peek().column , msg));
            return parser_error(msg);
        }

        inline Token Peek() const { return lexemes[index]; }
        inline Token Previous() const { return lexemes[index - 1]; }

        inline Token Advance() {
            if (!IsAtEnd()) ++index;
            return Previous();
        }

        inline Token Consume(Type t , const std::string& msg) {
            if (Peek().type == t) return Advance();
            throw GetError(ErrorLevel::FATAL , msg);
        }
        
        inline bool IsAtEnd() const {
            return index >= lexemes.size();
        }

        inline bool Check(Type type) {
            if (IsAtEnd()) return false;
            return Peek().type == type;
        }

        bool Match(std::vector<Type> types);

        void Synchronize();

        std::shared_ptr<ASTLiteral> CreateTypedLiteral(Token token);

        std::shared_ptr<ASTStmnt> ParseDecl();
        std::shared_ptr<ASTStmnt> ParseVarDecl();
        std::shared_ptr<ASTStmnt> ParseStmnt();
        std::shared_ptr<ASTStmnt> ParsePrintStmnt();
        std::shared_ptr<ASTStmnt> ParseExprStmnt();
        std::shared_ptr<ASTStmnt> ParseBlockStmnt();
        std::shared_ptr<ASTStmnt> ParseIfStmnt();
        std::shared_ptr<ASTStmnt> ParseWhileStmnt();
        std::shared_ptr<ASTStmnt> ParseForStmnt();
        std::shared_ptr<ASTStmnt> ParseFuncDecl(const std::string& kind);
        std::shared_ptr<ASTStmnt> ParseReturnStmnt();

        std::shared_ptr<ASTExpr> ParseExpr() { return ParseAssignment(); }
        std::shared_ptr<ASTExpr> ParseAssignment();
        std::shared_ptr<ASTExpr> ParseLogicalOr();
        std::shared_ptr<ASTExpr> ParseLogicalAnd();
        std::shared_ptr<ASTExpr> ParseEquality();
        std::shared_ptr<ASTExpr> ParseComparison();
        std::shared_ptr<ASTExpr> ParseTerm();
        std::shared_ptr<ASTExpr> ParseFactor();
        std::shared_ptr<ASTExpr> ParseUnary();
        std::shared_ptr<ASTExpr> ParseCall();
        std::shared_ptr<ASTExpr> FinishCall(std::shared_ptr<ASTExpr> callee);
        std::shared_ptr<ASTExpr> ParsePrimary();

        public:
            Parser(std::shared_ptr<ErrorHandler> err_handler , std::vector<Token> lexemes) 
                : err_handler(err_handler) , lexemes(lexemes) {}
            ~Parser() {}

            std::vector<std::shared_ptr<ASTStmnt>> Parse();
            
            inline const bool ParseFailed() const { return abort_parse; }
    };

}

#endif // !YL_PARSER_HPP