#include <iostream>

#include "ast.hpp"
#include "errors.hpp"

namespace ylang {

    void ASTPrinter::VisitLiteralExpr(ASTLiteral& expr) {
        expr.Print();
        std::cout << ")";
    }

    void ASTPrinter::VisitLogicalExpr(ASTLogicalExpr& expr) {
        std::cout << "(";
        expr.Print();
        std::cout << ")";
    }

    void ASTPrinter::VisitUnaryExpr(ASTUnaryExpr& expr) {
        std::cout << "(";
        expr.Print();
        std::cout << ")";
    }

    void ASTPrinter::VisitBinaryExpr(ASTBinaryExpr& expr) {
        std::cout << "(";
        expr.Print();
        std::cout << ")";
    }
    
    void ASTPrinter::VisitCallExpr(ASTCallExpr& expr) {
        std::cout << "(";
        expr.Print();
        std::cout << ")";
    }

    void ASTPrinter::VisitGroupingExpr(ASTGroupingExpr& expr) {
        std::cout << "(";
        expr.Print();
        std::cout << ")";
    }
    
    void ASTPrinter::VisitVarExpr(ASTVarExpr& expr) {
        std::cout << "(";
        expr.Print();
        std::cout << ")";
    }
    
    void ASTPrinter::VisitVarAssignExpr(ASTVarAssignExpr& expr) {
        std::cout << "(";
        expr.Print();
        std::cout << ")";
    }

    void ASTPrinter::VisitPrintStmnt(ASTPrintStmnt& stmnt) {
        std::cout << "(print ";
        stmnt.expr->Print();
        std::cout << ")";
    }

    void ASTPrinter::VisitExprStmnt(ASTExprStmnt& stmnt) {
        std::cout << "(";
        stmnt.expr->Print();
        std::cout << ")";
    }
    
    void ASTPrinter::VisitVarStmnt(ASTVarStmnt& stmnt) {
        std::cout << "(";
        stmnt.Print();
        std::cout << ")";
    }

    void ASTPrinter::VisitBlockStmnt(ASTBlockStmnt& stmnt) {
        std::cout << "\n{";
        stmnt.Print();
        std::cout << " }" << std::endl;
    }

    void ASTPrinter::VisitIfStmnt(ASTIfStmnt& stmnt) {
        std::cout << "(if (";
        stmnt.condition->Print();
        std::cout << ") { ";
        stmnt.then_branch->Print();
        std::cout << " }";
        if (stmnt.else_branch != nullptr) {
            std::cout << " else { ";
            stmnt.else_branch->Print();\
            std::cout << " }";
        }
        std::cout << ")";
    }

    void ASTPrinter::VisitWhileStmnt(ASTWhileStmnt& stmnt) {
        std::cout << "(while (";
        stmnt.condition->Print();
        std::cout << ") { ";
        stmnt.body->Print();
        std::cout << " }";
        std::cout << ")";
    }

    void ASTPrinter::VisitFunction(ASTFunction& stmnt) {
        std::cout << "(";
        stmnt.Print();
        std::cout << ")";
    }
    
    void ASTPrinter::VisitReturnStmnt(ASTReturnStmnt& stmnt) {
        std::cout << "(return ";
        stmnt.expr->Print();
        std::cout << ")";
    }

    void AST::Print() const {
        if (root == nullptr) return;
        std::shared_ptr<ExprVisitor> printer = std::make_shared<ASTPrinter>();
    }
    
    void ASTLiteral::Print() const {
        std::cout << this->literal.token;

    }

    void ASTLogicalExpr::Print() const {
        this->lhs->Print();
        std::cout << " " << this->op.token << " ";
        this->rhs->Print();
    }

    void ASTUnaryExpr::Print() const {
        std::cout << this->op.token << " ";
        this->expr->Print();
    }

    void ASTBinaryExpr::Print() const {
        this->lhs->Print();
        std::cout << " " << op.token << " ";
        this->rhs->Print();
    }
    
    void ASTCallExpr::Print() const {
        this->callee->Print();
        std::cout << "(";
        for (size_t i = 0; i < this->args.size(); i++) {
            this->args[i]->Print();
            if (i != this->args.size() - 1)
                std::cout << ", ";
        }
        std::cout << ")";
    }

    void ASTGroupingExpr::Print() const {
        this->expr->Print();
    }

    void ASTVarExpr::Print() const {
        std::cout << this->name.token;
    }

    void ASTVarAssignExpr::Print() const {
        std::cout << this->name.token << " = ";
        this->assignment->Print();
    }

    void ASTPrintStmnt::Print() const {
        std::cout << "print ";
        this->expr->Print();
    }

    void ASTExprStmnt::Print() const {
        this->expr->Print();
    }

    void ASTVarStmnt::Print() const {
        std::cout << this->type.token << " " << this->name.token;
        if (this->initializer != nullptr) {
            std::cout << " = "; 
            this->initializer->Print();
        }
    }
    
    void ASTBlockStmnt::Print() const {
        for (auto& stmnt : this->statements) {
            std::cout << "(";
            stmnt->Print();
            std::cout << ")";
        }
    }

    void ASTIfStmnt::Print() const {
        std::cout << "(if (";
        this->condition->Print();
        std::cout << ") { ";
        this->then_branch->Print();
        if (this->else_branch != nullptr) {
            std::cout << " } else { ";
            this->else_branch->Print();
        }
        std::cout << " })";
    }
    
    void ASTWhileStmnt::Print() const {
        std::cout << "(while (";
        this->condition->Print();
        std::cout << ") { ";
        this->body->Print();
        std::cout << " })";
    }

    void ASTFunction::Print() const {
        std::cout << name.token << "(";
        for (size_t i = 0; i < params.size(); i++) {
            std::cout << VarTypeStrings[static_cast<uint32_t>(params[i].type)] << " " << params[i].name;
            if (i != params.size() - 1)
                std::cout << ", ";
        }
        std::cout << ") { ";
        body->Print();
        std::cout << " })";
    }

    void ASTReturnStmnt::Print() const {
        std::cout << "(return ";
        if (expr != nullptr) this->expr->Print();
        std::cout << ")";
    }

}