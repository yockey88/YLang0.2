/**
 * \file ast.hpp
 * \version 0.0.1
 * \author Y
 * \brief contains the AST classes and definitions
*/

#ifndef YL_AST_HPP
#define YL_AST_HPP

#include <memory>
#include <iostream>
#include <vector>

#include "defines.hpp"

namespace ylang {

    class ASTLiteral;
    class ASTLogicalExpr;
    class ASTUnaryExpr;
    class ASTBinaryExpr;
    class ASTCallExpr;
    class ASTGroupingExpr;
    class ASTVarExpr;
    class ASTVarAssignExpr;
    class ASTPrintStmnt;
    class ASTExprStmnt;
    class ASTVarStmnt;
    class ASTBlockStmnt;
    class ASTIfStmnt;
    class ASTWhileStmnt;
    class ASTFunction;
    class ASTReturnStmnt;
    
    /// \class ExprVisitor
    /// \brief Abstract class used to visit a node in the AST
    // template <typename T>
    class ExprVisitor {
        public:
            virtual ~ExprVisitor() {}
            virtual void VisitLiteralExpr(ASTLiteral& expr) = 0;
            virtual void VisitLogicalExpr(ASTLogicalExpr& expr) = 0;
            virtual void VisitUnaryExpr(ASTUnaryExpr& expr) = 0;
            virtual void VisitBinaryExpr(ASTBinaryExpr& expr) = 0;
            virtual void VisitCallExpr(ASTCallExpr& expr) = 0;
            virtual void VisitGroupingExpr(ASTGroupingExpr& expr) = 0;
            virtual void VisitVarExpr(ASTVarExpr& expr) = 0;
            virtual void VisitVarAssignExpr(ASTVarAssignExpr& expr) = 0;
            virtual void VisitPrintStmnt(ASTPrintStmnt& stmnt) = 0;
            virtual void VisitExprStmnt(ASTExprStmnt& stmnt) = 0;
            virtual void VisitVarStmnt(ASTVarStmnt& stmnt) = 0;
            virtual void VisitBlockStmnt(ASTBlockStmnt& stmnt) = 0;
            virtual void VisitIfStmnt(ASTIfStmnt& stmnt) = 0;
            virtual void VisitWhileStmnt(ASTWhileStmnt& stmnt) = 0;
            virtual void VisitFunction(ASTFunction& stmnt) = 0;
            virtual void VisitReturnStmnt(ASTReturnStmnt& stmnt) = 0;
    };

    /// \class ASTPrinter
    /// \brief Class used to print the AST
    class ASTPrinter : public ExprVisitor {
        public:
            virtual ~ASTPrinter() override {}
            virtual void VisitLiteralExpr(ASTLiteral& expr) override;
            virtual void VisitLogicalExpr(ASTLogicalExpr& expr) override;
            virtual void VisitUnaryExpr(ASTUnaryExpr& expr) override;
            virtual void VisitBinaryExpr(ASTBinaryExpr& expr) override;
            virtual void VisitCallExpr(ASTCallExpr& expr) override;
            virtual void VisitGroupingExpr(ASTGroupingExpr& expr) override;
            virtual void VisitVarExpr(ASTVarExpr& expr) override;
            virtual void VisitVarAssignExpr(ASTVarAssignExpr& expr) override;
            virtual void VisitPrintStmnt(ASTPrintStmnt& stmnt) override;
            virtual void VisitExprStmnt(ASTExprStmnt& stmnt) override;
            virtual void VisitVarStmnt(ASTVarStmnt& stmnt) override;
            virtual void VisitBlockStmnt(ASTBlockStmnt& stmnt) override;
            virtual void VisitIfStmnt(ASTIfStmnt& stmnt) override;
            virtual void VisitWhileStmnt(ASTWhileStmnt& stmnt) override;
            virtual void VisitFunction(ASTFunction& stmnt) override;
            virtual void VisitReturnStmnt(ASTReturnStmnt& stmnt) override;
    };

    /// \class ASTNode
    /// \brief Abstract class used to represent a node in the AST
    class ASTNode {
        public:
            virtual ~ASTNode() {}
            virtual void Accept(ExprVisitor& visitor) = 0;
            virtual void Print() const = 0;
    };

    /// \class AST
    /// \brief Class used to represent the AST
    class AST {
        public:

            std::shared_ptr<ASTNode> root;

            AST() {}
            ~AST() {}

            /// \brief function used to print the AST
            void Print() const;
    };

    /// \class ASTExpr
    /// \brief Abstract class used to represent an expression in the AST
    class ASTExpr : public ASTNode {
        public:
            virtual ~ASTExpr() {}
            virtual void Accept(ExprVisitor& visitor) = 0;
            virtual void Print() const = 0;
            virtual Type GetType() const = 0;
    };

    /// \class ASTLiteral
    /// \brief Class used to represent a literal in the AST
    class ASTLiteral : public ASTExpr {
        public:
            Type type;
            Token literal;

            // ASTLiteral(Token literal) 
            //     : type(Type::ERROR_TOKEN) , literal(literal) {}
            ASTLiteral(Type type , Token literal)
                : type(type) , literal(literal) {}
            virtual ~ASTLiteral() override {}
            virtual void Accept(ExprVisitor& visitor) override {
                visitor.VisitLiteralExpr(*this);
            }
            virtual void Print() const override;
            virtual Type GetType() const override { return type; }
    };

    /// \class ASTLogicalExpr
    /// \brief Class used to represent a logical expression in the AST
    class ASTLogicalExpr : public ASTExpr {
        public:
            std::shared_ptr<ASTExpr> lhs;
            Token op;
            std::shared_ptr<ASTExpr> rhs;

            ASTLogicalExpr(std::shared_ptr<ASTExpr> lhs , Token op , std::shared_ptr<ASTExpr> rhs)
                : lhs(lhs) , op(op) , rhs(rhs) {}
            virtual ~ASTLogicalExpr() override {}
            virtual void Accept(ExprVisitor& visitor) override {
                visitor.VisitLogicalExpr(*this);
            }
            virtual void Print() const override;
            virtual Type GetType() const override { return op.type; }
    };

    /// \class ASTUnaryExpr
    /// \brief Class used to represent a unary expression in the AST
    class ASTUnaryExpr : public ASTExpr {
        public:
            Token op;
            std::shared_ptr<ASTExpr> expr;

            ASTUnaryExpr(Token op , std::shared_ptr<ASTExpr> expr) : op(op) , expr(expr) {}
            virtual ~ASTUnaryExpr() override {}
            virtual void Accept(ExprVisitor& visitor) override {
                visitor.VisitUnaryExpr(*this);
            }
            virtual void Print() const override;
            virtual Type GetType() const override { return op.type; }
    };

    /// \class ASTBinaryExpr
    /// \brief Class used to represent a binary expression in the AST
    class ASTBinaryExpr : public ASTExpr {
        public:      
            std::shared_ptr<ASTExpr> lhs;
            Token op;
            std::shared_ptr<ASTExpr> rhs;

            ASTBinaryExpr(std::shared_ptr<ASTExpr> lhs , Token op , std::shared_ptr<ASTExpr> rhs)
                : lhs(lhs) , op(op) , rhs(rhs) {}
            virtual ~ASTBinaryExpr() override {}
            virtual void Accept(ExprVisitor& visitor) override {
                visitor.VisitBinaryExpr(*this);
            }
            virtual void Print() const override;
            virtual Type GetType() const override { return op.type; }
    };

    /// \class ASTCallExpr
    /// \brief Class used to represent a call expression in the AST
    class ASTCallExpr : public ASTExpr {
        public:
            std::shared_ptr<ASTExpr> callee;
            Token paren;
            std::vector<std::shared_ptr<ASTExpr>> args;

            ASTCallExpr(std::shared_ptr<ASTExpr> callee , Token paren , std::vector<std::shared_ptr<ASTExpr>> args)
                : callee(callee) , paren(paren) , args(args) {}
            virtual ~ASTCallExpr() override {}
            virtual void Accept(ExprVisitor& visitor) override {
                visitor.VisitCallExpr(*this);
            }
            virtual void Print() const override;
            virtual Type GetType() const override { return callee->GetType(); }
    };

    /// \class ASTGroupingExpr
    /// \brief Class used to represent a grouping expression in the AST
    class ASTGroupingExpr : public ASTExpr {
        public:
            std::shared_ptr<ASTExpr> expr;

            ASTGroupingExpr(std::shared_ptr<ASTExpr> expr) : expr(expr) {}
            virtual ~ASTGroupingExpr() override {}
            virtual void Accept(ExprVisitor& visitor) override {
                visitor.VisitGroupingExpr(*this);
            }
            virtual void Print() const override;
            virtual Type GetType() const override { return expr->GetType(); }
    };

    /// \class ASTVarExpr
    /// \brief Class used to represent a variable expression in the AST
    class ASTVarExpr : public ASTExpr {
        public:
            Token name;

            ASTVarExpr(Token name) : name(name) {}
            virtual ~ASTVarExpr() override {}
            virtual void Accept(ExprVisitor& visitor) override {
                visitor.VisitVarExpr(*this);
            }
            virtual void Print() const override;
            virtual Type GetType() const override { return name.type; }
    };

    /// \class ASTVarAssignExpr
    /// \brief Class used to represent a variable assignment expression in the AST
    class ASTVarAssignExpr : public ASTExpr {
        public:
            Token name;
            std::shared_ptr<ASTExpr> assignment;

            ASTVarAssignExpr(Token name , std::shared_ptr<ASTExpr> assignment)
                : name(name) , assignment(assignment) {}
            virtual ~ASTVarAssignExpr() override {}
            virtual void Accept(ExprVisitor& visitor) override {
                visitor.VisitVarAssignExpr(*this);
            }
            virtual void Print() const override;
            virtual Type GetType() const override { return name.type; }
    };

    /// \class ASTStmnt
    /// \brief Abstract class used to represent a statement in the AST
    class ASTStmnt : public ASTNode {
        public:
            virtual ~ASTStmnt() {}
            virtual void Accept(ExprVisitor& visitor) = 0;
            virtual void Print() const = 0;
    };

    /// \class ASTPrintStmnt
    /// \brief Class used to represent a print statement in the AST
    class ASTPrintStmnt : public ASTStmnt {
        public:
            std::shared_ptr<ASTExpr> expr;

            ASTPrintStmnt(std::shared_ptr<ASTExpr> expr) : expr(expr) {}
            virtual ~ASTPrintStmnt() override {}
            virtual void Accept(ExprVisitor& visitor) override {
                visitor.VisitPrintStmnt(*this);
            }
            virtual void Print() const override;
    };

    /// \class ASTExprStmnt
    /// \brief Class used to represent an expression statement in the AST
    class ASTExprStmnt : public ASTStmnt {
        public:
            std::shared_ptr<ASTExpr> expr;

            ASTExprStmnt(std::shared_ptr<ASTExpr> expr) : expr(expr) {}
            virtual ~ASTExprStmnt() override {}
            virtual void Accept(ExprVisitor& visitor) override {
                visitor.VisitExprStmnt(*this);
            }
            virtual void Print() const override;
    };

    /// \class ASTVarStmnt
    /// \brief Class used to represent a variable statement in the AST
    class ASTVarStmnt : public ASTStmnt {
        public:
            Token type;
            Token name;
            std::shared_ptr<ASTExpr> initializer;

            ASTVarStmnt(Token type , Token name , std::shared_ptr<ASTExpr> initializer)
                : type(type) , name(name) , initializer(initializer) {}
            virtual ~ASTVarStmnt() override {}
            virtual void Accept(ExprVisitor& visitor) override {
                visitor.VisitVarStmnt(*this);
            }
            virtual void Print() const override;
    };

    /// \class ASTBlockStmnt
    /// \brief Class used to represent a block statement in the AST
    class ASTBlockStmnt : public ASTStmnt {
        public:
            std::vector<std::shared_ptr<ASTStmnt>> statements;

            ASTBlockStmnt(std::vector<std::shared_ptr<ASTStmnt>> statements)
                : statements(statements) {}
            virtual ~ASTBlockStmnt() override {}
            virtual void Accept(ExprVisitor& visitor) override {
                visitor.VisitBlockStmnt(*this);
            }
            virtual void Print() const override;
    };

    /// \class ASTIfStmnt
    /// \brief Class used to represent an if statement in the AST
    class ASTIfStmnt : public ASTStmnt {
        public:
            std::shared_ptr<ASTExpr> condition;
            std::shared_ptr<ASTStmnt> then_branch;
            std::shared_ptr<ASTStmnt> else_branch;

            ASTIfStmnt(std::shared_ptr<ASTExpr> condition ,
                       std::shared_ptr<ASTStmnt> then_branch ,
                       std::shared_ptr<ASTStmnt> else_branch)
                : condition(condition) , then_branch(then_branch) , else_branch(else_branch) {}
            virtual ~ASTIfStmnt() override {}
            virtual void Accept(ExprVisitor& visitor) override {
                visitor.VisitIfStmnt(*this);
            }
            virtual void Print() const override;
    };

    /// \class ASTWhileStmnt
    /// \brief Class used to represent a while statement in the AST
    class ASTWhileStmnt : public ASTStmnt {
        public:
            std::shared_ptr<ASTExpr> condition;
            std::shared_ptr<ASTStmnt> body;

            ASTWhileStmnt(std::shared_ptr<ASTExpr> condition , std::shared_ptr<ASTStmnt> body)
                : condition(condition) , body(body) {}
            virtual ~ASTWhileStmnt() override {}
            virtual void Accept(ExprVisitor& visitor) override {
                visitor.VisitWhileStmnt(*this);
            }
            virtual void Print() const override;
    };

    /// \class ASTFunction
    /// \brief Class used to represent a function in the AST
    class ASTFunction : public ASTStmnt {
        public:
            Token name;
            std::vector<Variable> params;
            std::shared_ptr<ASTStmnt> body;

            ASTFunction(Token name , std::vector<Variable> params , std::shared_ptr<ASTStmnt> body)
                : name(name) , params(params) , body(body) {}
            virtual ~ASTFunction() override {}
            virtual void Accept(ExprVisitor& visitor) override {
                visitor.VisitFunction(*this);
            }
            virtual void Print() const override;
    };

    /// \class ASTReturnStmnt
    /// \brief Class used to represent a return statement in the AST
    class ASTReturnStmnt : public ASTStmnt {
        public:
            Token keyword;
            std::shared_ptr<ASTExpr> expr;

            ASTReturnStmnt(Token keyword , std::shared_ptr<ASTExpr> expr)
                : keyword(keyword) , expr(expr) {}
            virtual ~ASTReturnStmnt() override {}
            virtual void Accept(ExprVisitor& visitor) override {
                visitor.VisitReturnStmnt(*this);
            }
            virtual void Print() const override;
    };

}

#endif // !YL_AST_HPP