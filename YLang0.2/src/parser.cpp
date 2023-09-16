#include "parser.hpp"

#include <iostream>

namespace ylang {

    bool Parser::Match(std::vector<Type> types) {
        for (auto& type : types) {
            if (Check(type)) {
                Advance();
                return true;
            }
        }
        return false;
    }

    void Parser::Synchronize() {
        Advance();
        while (!IsAtEnd()) {
            if (Previous().type == Type::OP_SEMICOLON) return;
            switch (Peek().type) {
                case Type::KW_CLASS:  [[fallthrough]];
                case Type::KW_FN:     [[fallthrough]];
                case Type::KW_I8:     [[fallthrough]];
                case Type::KW_I16:    [[fallthrough]];
                case Type::KW_I32:    [[fallthrough]];
                case Type::KW_I64:    [[fallthrough]];
                case Type::KW_U8:     [[fallthrough]];
                case Type::KW_U16:    [[fallthrough]];
                case Type::KW_U32:    [[fallthrough]];
                case Type::KW_U64:    [[fallthrough]];
                case Type::KW_F32:    [[fallthrough]];
                case Type::KW_F64:    [[fallthrough]];
                case Type::KW_BOOL:   [[fallthrough]];
                case Type::KW_CHAR:   [[fallthrough]];
                case Type::KW_STRING: [[fallthrough]];
                case Type::KW_VOID:   [[fallthrough]];
                case Type::KW_FOR:    [[fallthrough]];
                case Type::KW_IF:     [[fallthrough]];
                case Type::KW_WHILE:  [[fallthrough]];
                case Type::KW_RETURN: return;
                default:
                    Advance();
            }
        }
        err_handler->SubmitError(GetError(ErrorLevel::FATAL , "Unexpected end of file"));
        abort_parse = true;
    }
    
    std::shared_ptr<ASTLiteral> Parser::CreateTypedLiteral(Token token) {
        return nullptr;
    }

    std::shared_ptr<ASTStmnt> Parser::ParseDecl() {
        try {
            if (Match({ Type::KW_FN })) return ParseFuncDecl("function");
            if (Match({ Type::KW_I8 , Type::KW_I16 , Type::KW_I32 , Type::KW_I64 ,
                        Type::KW_U8 , Type::KW_U16 , Type::KW_U32 , Type::KW_U64 ,
                        Type::KW_F32 , Type::KW_F64 ,
                        Type::KW_BOOL ,
                        Type::KW_CHAR , Type::KW_STRING })) {
                var_type = Previous().type;
                return ParseVarDecl();
            }
            return ParseStmnt();
        } catch (parser_error& e) {
            err_handler->SubmitError(e);
            Synchronize();
            return nullptr;
        }
    }

    std::shared_ptr<ASTStmnt> Parser::ParseVarDecl() {
        Token type = Previous();
        Token name = Consume(Type::IDENTIFIER , "Expected identifier after type");
        std::shared_ptr<ASTExpr> expr = nullptr;
        if (Match({ Type::OP_ASSIGN })) { 
            in_var_decl = true;
            expr = ParseExpr();
            in_var_decl = false;
        }
        Consume(Type::OP_SEMICOLON , "Expected ';' after variable declaration");
        return std::make_shared<ASTVarStmnt>(type , name , expr);
    }

    std::shared_ptr<ASTStmnt> Parser::ParseStmnt() {
        if (Match({ Type::KW_FOR })) return ParseForStmnt();
        if (Match({ Type::KW_IF })) return ParseIfStmnt();
        if (Match({ Type::KW_PRINT })) return ParsePrintStmnt();
        if (Match({ Type::KW_RETURN })) return ParseReturnStmnt();
        if (Match({ Type::KW_WHILE })) return ParseWhileStmnt();
        if (Match({ Type::OP_OPEN_BRACE })) return ParseBlockStmnt();
        return ParseExprStmnt();
    }
        
    std::shared_ptr<ASTStmnt> Parser::ParsePrintStmnt() {
        std::vector<Type> kw_types = {
            Type::KW_I8 , Type::KW_I16 , Type::KW_I32 , Type::KW_I64 ,
            Type::KW_U8 , Type::KW_U16 , Type::KW_U32 , Type::KW_U64 ,
            Type::KW_F32 , Type::KW_F64 ,
            Type::KW_BOOL ,
            Type::KW_CHAR , Type::KW_STRING
        };
        for (auto& type : kw_types) {
            if (Check(type)) {
                var_type = type;
                break;
            }
        }
        std::shared_ptr<ASTExpr> expr = ParseExpr();
        Consume(Type::OP_SEMICOLON , "Expected ';' after expression");
        return std::make_shared<ASTPrintStmnt>(expr);
    }

    std::shared_ptr<ASTStmnt> Parser::ParseExprStmnt() {
        std::shared_ptr<ASTExpr> expr = ParseExpr();
        Consume(Type::OP_SEMICOLON , "Expected ';' after expression");
        return std::make_shared<ASTExprStmnt>(expr);
    }
    
    std::shared_ptr<ASTStmnt> Parser::ParseBlockStmnt() {
        std::vector<std::shared_ptr<ASTStmnt>> statements{};
        while (!Check(Type::OP_CLOSE_BRACE) && !IsAtEnd()) {
            std::shared_ptr<ASTStmnt> stmnt = ParseDecl();
            if (stmnt != nullptr) statements.push_back(stmnt);
            if (abort_parse) break;
        }
        Consume(Type::OP_CLOSE_BRACE , "Expected '}' after block");
        return std::make_shared<ASTBlockStmnt>(statements);
    }

    std::shared_ptr<ASTStmnt> Parser::ParseIfStmnt() {
        bool has_paren = false;
        if (Check(Type::OP_OPEN_PAREN)) {
            Consume(Type::OP_OPEN_PAREN , "Undefined behavior, check for '(' after 'if' successfull");
            has_paren = true;
        }
        std::shared_ptr<ASTExpr> condition = ParseExpr();

        if (has_paren)
            Consume(Type::OP_CLOSE_PAREN , "Expected ')' after condition opened with '('");
        
        std::shared_ptr<ASTStmnt> then_branch = ParseStmnt();

        std::shared_ptr<ASTStmnt> else_branch = nullptr;
        if (Match({ Type::KW_ELSE })) else_branch = ParseStmnt();

        return std::make_shared<ASTIfStmnt>(condition , then_branch , else_branch);
    }
    
    std::shared_ptr<ASTStmnt> Parser::ParseWhileStmnt() {
        bool has_paren = false;
        if (Check(Type::OP_OPEN_PAREN)) {
            Consume(Type::OP_OPEN_PAREN , "Undefined behavior, check for '(' after 'while' successfull");
            has_paren = true;
        }

        std::shared_ptr<ASTExpr> condition = ParseExpr();

        if (has_paren)
            Consume(Type::OP_CLOSE_PAREN , "Expected ')' after condition opened with '('");
        
        std::shared_ptr<ASTStmnt> body = ParseStmnt();
        
        return std::make_shared<ASTWhileStmnt>(condition , body);
    }
    
    std::shared_ptr<ASTStmnt> Parser::ParseForStmnt() {
        Consume(Type::OP_OPEN_PAREN , "Expected '(' after 'for'");
        std::shared_ptr<ASTStmnt> initializer = nullptr;
        if (Match({ Type::OP_SEMICOLON })) {
            initializer = nullptr;
        } else if (Match({ Type::KW_I8 , Type::KW_I16 , Type::KW_I32 , Type::KW_I64 ,
                        Type::KW_U8 , Type::KW_U16 , Type::KW_U32 , Type::KW_U64 ,
                        Type::KW_F32 , Type::KW_F64 ,
                        Type::KW_BOOL ,
                        Type::KW_CHAR , Type::KW_STRING })) {
            var_type = Previous().type;
            initializer = ParseVarDecl();
        } else {
            initializer = ParseExprStmnt();
        }

        std::shared_ptr<ASTExpr> condition = nullptr;
        if (!Check(Type::OP_SEMICOLON)) condition = ParseExpr();
        Consume(Type::OP_SEMICOLON , "Expected ';' after loop condition");

        std::shared_ptr<ASTExpr> increment = nullptr;
        if (!Check(Type::OP_CLOSE_PAREN)) increment = ParseExpr();
        Consume(Type::OP_CLOSE_PAREN , "Expected ')' after for clauses");

        std::shared_ptr<ASTStmnt> body = ParseStmnt();

        if (increment != nullptr) {
            std::vector<std::shared_ptr<ASTStmnt>> statements{};
            statements.push_back(body);
            statements.push_back(std::make_shared<ASTExprStmnt>(increment));
            body = std::make_shared<ASTBlockStmnt>(statements);
        }

        if (condition == nullptr) 
            condition = std::make_shared<ASTLiteral>(
                Type::KW_TRUE , Token{ Type::KW_TRUE , Peek().line , Peek().column , "true" }
            );

        body = std::make_shared<ASTWhileStmnt>(condition , body);

        if (initializer != nullptr) {
            std::vector<std::shared_ptr<ASTStmnt>> statements{};
            statements.push_back(initializer);
            statements.push_back(body);
            body = std::make_shared<ASTBlockStmnt>(statements);
        }

        return body;
    }
    
    std::shared_ptr<ASTStmnt> Parser::ParseFuncDecl(const std::string& kind) {
        Token name = Consume(Type::IDENTIFIER , "Expected " + kind + " name");
        Consume(Type::OP_OPEN_PAREN , "Expected '(' after " + kind + " name");

        std::vector<Variable> params{};
        if (!Check(Type::OP_CLOSE_PAREN)) {
            do {
                if (params.size() >= 255) {
                    err_handler->HandleParserError(
                        GetError(ErrorLevel::FATAL , "Cannot have more than 255 parameters")
                    );
                }
                Variable param{};
                // horribly long vector to check for type
                // this throws so we can just 'one-line' it (i know it's not one line bite me)
                if (!Match({ Type::KW_I8 , Type::KW_I16 , Type::KW_I32 , Type::KW_I64 ,
                                    Type::KW_U8 , Type::KW_U16 , Type::KW_U32 , Type::KW_U64 ,
                                    Type::KW_F32 , Type::KW_F64 ,
                                    Type::KW_BOOL ,
                                    Type::KW_CHAR , Type::KW_STRING })) throw GetError(ErrorLevel::FATAL , "Expected parameter type");
                param.type = VarTypeFromToken(Previous().type);
                param.name = Consume(Type::IDENTIFIER , "Expected parameter name").token;
                params.push_back(param);
            } while (Match({ Type::OP_COMMA }));
        }
        Consume(Type::OP_CLOSE_PAREN , "Expected ')' after parameters");
        Consume(Type::OP_OPEN_BRACE , "Expected '{' before " + kind + " body");
        std::shared_ptr<ASTStmnt> body = ParseBlockStmnt();

        if (body == nullptr)
            throw GetError(ErrorLevel::FATAL , "Expected " + kind + " body");

        for (auto& param : params) {
            if (param.type == VarType::VOID) {
                err_handler->HandleParserError(
                    GetError(ErrorLevel::FATAL , "Cannot have void parameters")
                );
            }
        }

        return std::make_shared<ASTFunction>(name , params , body);
    }
    
    std::shared_ptr<ASTStmnt> Parser::ParseReturnStmnt() {
        Token keyword = Previous();
        std::shared_ptr<ASTExpr> value = nullptr;
        if (!Check(Type::OP_SEMICOLON)) value = ParseExpr();
        Consume(Type::OP_SEMICOLON , "Expected ';' after return value");
        return std::make_shared<ASTReturnStmnt>(keyword , value);
    }
    
    std::shared_ptr<ASTExpr> Parser::ParseAssignment() {
        std::shared_ptr<ASTExpr> expr = ParseLogicalOr();
        if (Match({ Type::OP_ASSIGN })) {
            Token op = Previous();
            std::shared_ptr<ASTExpr> value = ParseAssignment();
            if (expr->GetType() == Type::IDENTIFIER) {
                Token name = std::static_pointer_cast<ASTVarExpr>(expr)->name;
                return std::make_shared<ASTVarAssignExpr>(name , value);
            }
            throw GetError(ErrorLevel::FATAL , "Invalid assignment target");
        }
        return expr;
    }

    std::shared_ptr<ASTExpr> Parser::ParseLogicalOr() {
        std::shared_ptr<ASTExpr> expr = ParseLogicalAnd();
        while (Match({ Type::OP_LOGICAL_OR })) {
            Token op = Previous();
            std::shared_ptr<ASTExpr> right = ParseLogicalAnd();
            expr = std::make_shared<ASTLogicalExpr>(expr , op , right);
        }
        return expr;
    }

    std::shared_ptr<ASTExpr> Parser::ParseLogicalAnd() {
        std::shared_ptr<ASTExpr> expr = ParseEquality();
        while (Match({ Type::OP_LOGICAL_AND })) {
            Token op = Previous();
            std::shared_ptr<ASTExpr> right = ParseEquality();
            expr = std::make_shared<ASTLogicalExpr>(expr , op , right);
        }
        return expr;
    }

    std::shared_ptr<ASTExpr> Parser::ParseEquality() {
        std::shared_ptr<ASTExpr> expr = ParseComparison();
        while (Match({ Type::OP_EQUALS , Type::OP_NOT_EQUAL })) {
            Token op = Previous();
            std::shared_ptr<ASTExpr> right = ParseComparison();
            expr = std::make_shared<ASTBinaryExpr>(expr , op , right);
        }
        return expr;
    }   

    std::shared_ptr<ASTExpr> Parser::ParseComparison() {
        std::shared_ptr<ASTExpr> expr = ParseTerm();
        while (Match({ Type::OP_GREATER_THAN , Type::OP_GREATER_THAN_EQUAL , Type::OP_LESS_THAN , Type::OP_LESS_THAN_EQUAL })) {
            Token op = Previous();
            std::shared_ptr<ASTExpr> right = ParseTerm();
            expr = std::make_shared<ASTBinaryExpr>(expr , op , right);
        }
        return expr;
    }

    std::shared_ptr<ASTExpr> Parser::ParseTerm() {
        std::shared_ptr<ASTExpr> expr = ParseFactor();
        while (Match({ Type::OP_ADD , Type::OP_SUB })) {
            Token op = Previous();
            std::shared_ptr<ASTExpr> right = ParseFactor();
            expr = std::make_shared<ASTBinaryExpr>(expr , op , right);
        }
        return expr;
    }

    std::shared_ptr<ASTExpr> Parser::ParseFactor() {
        std::shared_ptr<ASTExpr> expr = ParseUnary();
        while (Match({ Type::OP_MUL , Type::OP_DIV })) {
            Token op = Previous();
            std::shared_ptr<ASTExpr> right = ParseUnary();
            expr = std::make_shared<ASTBinaryExpr>(expr , op , right);
        }
        return expr;
    }

    std::shared_ptr<ASTExpr> Parser::ParseUnary() {
        if (Match({ Type::OP_SUB , Type::OP_EXCLAMATION })) {
            Token op = Previous();
            std::shared_ptr<ASTExpr> right = ParseUnary();
            return std::make_shared<ASTUnaryExpr>(op , right);
        }
        return ParseCall();
    }
    
    std::shared_ptr<ASTExpr> Parser::ParseCall() {
        std::shared_ptr<ASTExpr> expr = ParsePrimary();
        while (true) {
            if (Match({ Type::OP_OPEN_PAREN })) {
                expr = FinishCall(expr);
            } else {
                break;
            }
        }
        return expr;
    }

    std::shared_ptr<ASTExpr> Parser::FinishCall(std::shared_ptr<ASTExpr> callee) {
        std::vector<std::shared_ptr<ASTExpr>> args{};
        if (!Check(Type::OP_CLOSE_PAREN)) {
            do {
                if (args.size() >= 255) {
                    err_handler->HandleParserError(
                        GetError(ErrorLevel::FATAL , "Cannot have more than 255 arguments")
                    );
                }
                args.push_back(ParseExpr());
            } while (Match({ Type::OP_COMMA }));
        }
        Token paren = Consume(Type::OP_CLOSE_PAREN , "Expected ')' after arguments");
        return std::make_shared<ASTCallExpr>(callee , paren , args);
    }

    std::shared_ptr<ASTExpr> Parser::ParsePrimary() {
        int line = Peek().line;
        int column = Peek().column;
        if (Match({ Type::KW_FALSE })) return std::make_shared<ASTLiteral>(Type::KW_FALSE , Peek());
        if (Match({ Type::KW_TRUE })) return std::make_shared<ASTLiteral>(Type::KW_TRUE , Peek());
        if (Match({ Type::KW_NULL })) return std::make_shared<ASTLiteral>(Type::KW_NULL , Peek());

        if (Match({ Type::LIT_INT , Type::LIT_HEX , Type::LIT_FLOAT , Type::LIT_CHAR , Type::LIT_STRING })) {
            Type type = Previous().type;
            if (in_var_decl) type = var_type;
            return std::make_shared<ASTLiteral>(type , Previous());
        }

        if (Match({ Type::IDENTIFIER })) {
            Token name = Previous();
            return std::make_shared<ASTVarExpr>(name);
        }
        
        if (Match({ Type::OP_OPEN_PAREN })) {
            std::shared_ptr<ASTExpr> expr = ParseExpr();
            Consume(Type::OP_CLOSE_PAREN , "Expected ')' after expression");
            return std::make_shared<ASTGroupingExpr>(expr);
        }

        throw GetError(ErrorLevel::FATAL , "Expected expression");
    }

    std::vector<std::shared_ptr<ASTStmnt>> Parser::Parse() {
        if (IsAtEnd()) return {};
        if (Peek().type != Type::START_OF_FILE) 
            throw GetError(ErrorLevel::FATAL , "Expected start of file");
        
        index = 1;

        while (!IsAtEnd() && Peek().type != Type::END_OF_FILE) {
            std::shared_ptr<ASTStmnt> stmnt = ParseDecl();
            if (stmnt != nullptr) statements.push_back(stmnt);
            if (abort_parse) break;
        }
        return statements;
    }

}