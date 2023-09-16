/**
 * \file interpreter.hpp
 * \version 0.0.1
 * \author Y
 * \brief contains the interpreter class
*/

#ifndef YL_INTERPRETER_HPP
#define YL_INTERPRETER_HPP

#include <string>
#include <stack>
#include <queue>
#include <map>
#include <unordered_map>
#include <memory>
#include <variant>

#include "errors.hpp"
#include "ast.hpp"

namespace ylang {

    class Interpreter;
    class Environment;
    
    class Callable {
        public:
            virtual ~Callable() {}
            virtual Variable Call(Interpreter& interpreter , const std::vector<Variable>& args ,  bool is_initializer = false) = 0;
            virtual size_t Arity() const = 0;
            virtual std::string ToString() const = 0;
    };

    class Function : public Callable {
        std::string name;
        std::vector<Variable> params{};
        std::shared_ptr<ASTStmnt> definition = nullptr;
        std::shared_ptr<Environment> closure = nullptr;
        bool is_initializer = false;

        public:
            Function(std::shared_ptr<ASTStmnt> definition , std::shared_ptr<Environment> closure ,
                        std::vector<Variable> params , const std::string& name = "") 
                : definition(definition) , closure(closure) , params(params) , name(name) {}
            virtual Variable Call(Interpreter& interpreter , const std::vector<Variable>& args , bool is_initializer = false) override;
            virtual size_t Arity() const override;
            virtual std::string ToString() const override;
    };

    struct Scope {
        std::string name = "";
        std::shared_ptr<Scope> child = nullptr;
        std::unordered_map<std::string , std::shared_ptr<Variable>> variables{};
        std::unordered_map<std::string , std::shared_ptr<Callable>> functions{};
    };

    class Environment {
        Environment(const Environment&) = delete;
        Environment(Environment&&) = delete;
        Environment& operator=(const Environment&) = delete;
        Environment& operator=(Environment&&) = delete;

        std::shared_ptr<Scope> current_scope = nullptr;

        uint32_t scope_count = 0;

        public:
            Environment();
            ~Environment();

            void PrintScope() const;

            void PushScope(const std::string& name = "");
            void PopScope();

            void AddVar(const std::string& name , const Variable& var);
            void SetVar(const std::string& name , const Variable& var);

            void AddFunction(const std::string& name , std::shared_ptr<Callable>& func);

            Variable* GetVarFromScope(const std::string& name);
            Variable* GetVar(const std::string& name);

            Callable* GetCallable(const std::string& name);
    };

    struct VM {
        static std::shared_ptr<ErrorHandler> err_handler;
        static std::stack<Variable> vstack;
        static std::shared_ptr<Environment> env;
        static void Interpret(const std::vector<std::shared_ptr<ASTStmnt>>& ast);
    };

    class return_value : public std::exception {
        Variable value;
        public:
            return_value(const Variable& value) : value(value) {}
            Variable GetValue() const { return value; }
    };

    class Interpreter : public ExprVisitor {

        return_value Return(const Variable& value);

        Variable MakeVar(VarType type , const VarValue& val);
        Variable* GetVar(const std::string& name);

        void EvaluateBinaryExpr(Type op , const Variable& lhs , const Variable& rhs);

        bool IsTruthy(const Variable& val);

        bool call_expr = false;

        public:
            virtual ~Interpreter() override {}
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
            
            void ExecuteBlock(std::shared_ptr<ASTStmnt> body , std::shared_ptr<Environment> closure);
    };

}

#endif // !YL_INTERPRETER_HPP