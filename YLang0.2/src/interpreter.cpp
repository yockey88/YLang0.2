#include "interpreter.hpp"

#include <stdexcept>
#include <iostream>
#include <cassert>
#include <limits>

#include "errors.hpp"
#include "defines.hpp"

namespace ylang {

    Variable Function::Call(Interpreter& interpreter , const std::vector<Variable>& args , bool is_initializer) {
        
        if (args.size() != Arity())
            throw interpreter_error(
                "Expected " + std::to_string(Arity()) +
                " arguments but got " + std::to_string(args.size())
            );

        closure->PushScope(name);

        for (size_t i = 0; i < args.size(); ++i) {
            closure->AddVar(params[i].name , args[i]);
        }

        Variable ret_var{};
        try {
            interpreter.ExecuteBlock(definition , closure);
        } catch (const return_value& ret) {
            ret_var = ret.GetValue();
        }
        
        closure->PopScope();
        return ret_var;
    }

    size_t Function::Arity() const {
        return params.size();
    }

    std::string Function::ToString() const {
        return "<fn " + name + ">";
    }

    Environment::Environment() {
        std::shared_ptr<Scope> global_scope = std::make_shared<Scope>();
        global_scope->name = "global";
        global_scope->child = nullptr;
        current_scope = global_scope;
    }

    Environment::~Environment() {}

    void Environment::PrintScope() const {
        std::shared_ptr<Scope> scope = current_scope;
        while (scope != nullptr) {
            std::cout << scope->name << " :: { " << std::endl;
            for (auto& var : scope->variables) {
                std::cout << "\t" << var.first << " : "; 
                
                std::visit([](auto&& arg) {
                    std::cout << arg;
                } , var.second->value);
                
                std::cout << std::endl;
            }
            std::cout << "} ->" << std::endl;
            scope = scope->child;
        }
    }

    void Environment::PushScope(const std::string& name) {
        std::shared_ptr scope = std::make_shared<Scope>();
        if (name == "") {
            scope->name = "scope_" + std::to_string(scope_count++);
        } else {
            scope->name = name;
        }
        if (current_scope == nullptr) {
            current_scope = scope;
        } else {
            scope->child = current_scope;
            current_scope = scope;
        }
    }

    void Environment::PopScope() {
        if (current_scope == nullptr) {
            return;
        }
        current_scope = current_scope->child;
        --scope_count;
    }
    
    void Environment::AddVar(const std::string& name , const Variable& var) {
        current_scope->variables[name] = std::make_shared<Variable>(var);
    }
    
    void Environment::SetVar(const std::string& name , const Variable& var) {
        std::shared_ptr<Scope> scope = current_scope;
        while (scope != nullptr) {
            if (scope->variables.find(name) != scope->variables.end()) {
                scope->variables[name] = std::make_shared<Variable>(var);
                return;
            }
            scope = scope->child;
        }
    }
    
    void Environment::AddFunction(const std::string& name , std::shared_ptr<Callable>& func) {
        current_scope->functions[name] = func;
    }

    Variable* Environment::GetVarFromScope(const std::string& name) {
        std::shared_ptr<Scope> scope = current_scope;
        if (scope->variables.find(name) != scope->variables.end()) {
            return scope->variables[name].get();
        }
        return nullptr;
    }

    Variable* Environment::GetVar(const std::string& name) {
        std::shared_ptr<Scope> scope = current_scope;
        while (scope != nullptr) {
            if (scope->variables.find(name) != scope->variables.end()) {
                return scope->variables[name].get();
            }
            scope = scope->child;
        }
        return nullptr;
    }  

    Callable* Environment::GetCallable(const std::string& name) {
        std::shared_ptr<Scope> scope = current_scope;
        while (scope != nullptr) {
            if (scope->functions.find(name) != scope->functions.end()) {
                return scope->functions[name].get();
            }
            scope = scope->child;
        }
        return nullptr;
    }
    
    std::shared_ptr<ErrorHandler> VM::err_handler = nullptr;
    std::stack<Variable> VM::vstack{};
    std::shared_ptr<Environment> VM::env = nullptr;

    void VM::Interpret(const std::vector<std::shared_ptr<ASTStmnt>>& ast) {
        err_handler = std::make_shared<ErrorHandler>();
        env = std::make_shared<Environment>();
        
        try {
            std::shared_ptr<ExprVisitor> interpreter = std::make_shared<Interpreter>();
            for (auto& stmnt : ast)
                stmnt->Accept(*(interpreter.get()));
        } catch (const interpreter_error& e) {
            err_handler->HandleRuntimeError(Error(ErrorLevel::FATAL , 0 , 0 , e.what()));
            return;
        }
    }

    return_value Interpreter::Return(const Variable& value) {
        return return_value(value);
    }

    Variable Interpreter::MakeVar(VarType type , const VarValue& val) {
        return Variable{ type , val };
    }

    Variable* Interpreter::GetVar(const std::string& name) {
        Variable* var = nullptr;
        var = VM::env->GetVar(name);
        if (var == nullptr)
            throw interpreter_error("Undefined variable " + name);
        return var;
    }

    /// \todo Be better than this :(
    /// \note I am so sorry for this, this is so bad, i am not proud of this
    void Interpreter::EvaluateBinaryExpr(Type op , const Variable& lhs , const Variable& rhs) {
        try {
            switch (op) {
                case Type::OP_NOT_EQUAL: VM::vstack.push(
                    MakeVar(VarType::BOOL , lhs.value != rhs.value)
                ); break;
                case Type::OP_EQUALS: VM::vstack.push(
                    MakeVar(VarType::BOOL , lhs.value == rhs.value)
                ); break;
                case Type::OP_GREATER_THAN: VM::vstack.push(
                    MakeVar(VarType::BOOL , lhs.value > rhs.value)
                ); break;
                case Type::OP_GREATER_THAN_EQUAL: VM::vstack.push(
                    MakeVar(VarType::BOOL , lhs.value >= rhs.value)
                ); break;
                case Type::OP_LESS_THAN: VM::vstack.push(
                    MakeVar(VarType::BOOL , lhs.value < rhs.value)
                ); break;
                case Type::OP_LESS_THAN_EQUAL: VM::vstack.push(
                    MakeVar(VarType::BOOL , lhs.value <= rhs.value)
                ); break;
                case Type::OP_ADD: VM::vstack.push(
                        MakeVar(
                            static_cast<VarType>(
                                std::max(static_cast<uint8_t>(lhs.type) , static_cast<uint8_t>(rhs.type))
                            ) , lhs.value + rhs.value
                        )
                ); break;
                case Type::OP_SUB: VM::vstack.push(
                    MakeVar(
                        static_cast<VarType>(
                            std::max(static_cast<uint8_t>(lhs.type) , static_cast<uint8_t>(rhs.type))
                        ) , lhs.value - rhs.value
                    )
                ); break;
                case Type::OP_MUL: VM::vstack.push(
                    MakeVar(
                        static_cast<VarType>(
                            std::max(static_cast<uint8_t>(lhs.type) , static_cast<uint8_t>(rhs.type)) 
                        ) , lhs.value * rhs.value
                    )
                ); break;
                case Type::OP_DIV: VM::vstack.push(
                    MakeVar(
                        static_cast<VarType>(
                            std::max(static_cast<uint8_t>(lhs.type) , static_cast<uint8_t>(rhs.type))
                        ) , lhs.value / rhs.value
                    )
                ); break;
                case Type::OP_MOD: VM::vstack.push(
                    MakeVar(
                        static_cast<VarType>(
                            std::max(static_cast<uint8_t>(lhs.type) , static_cast<uint8_t>(rhs.type))
                        ) , lhs.value % rhs.value
                    )
                ); break;
                default:
                    throw interpreter_error("Invalid binary operator applied to ints");
            }
        } catch (const std::runtime_error& e) {
            throw interpreter_error(e.what());
        }
    }
    
    bool Interpreter::IsTruthy(const Variable& val) {
        switch (val.type) {
            case VarType::I8:   return std::get<int8_t>(val.value) != 0;
            case VarType::I16:  return std::get<int16_t>(val.value) != 0;
            case VarType::I32:  return std::get<int32_t>(val.value) != 0;
            case VarType::I64:  return std::get<int64_t>(val.value) != 0;
            case VarType::U8:   return std::get<uint8_t>(val.value) != 0;
            case VarType::U16:  return std::get<uint16_t>(val.value) != 0;
            case VarType::U32:  return std::get<uint32_t>(val.value) != 0;
            case VarType::U64:  return std::get<uint64_t>(val.value) != 0;
            case VarType::F32:  return std::get<float>(val.value) != 0;
            case VarType::F64:  return std::get<double>(val.value) != 0;
            case VarType::BOOL: return std::get<bool>(val.value);
            case VarType::CHAR: return std::get<char>(val.value) != '\0';
            case VarType::STRING: return !std::get<std::string>(val.value).empty();
            default:
                throw interpreter_error("Cannot convert type to boolean");
        }
        return true;
    }

    void Interpreter::VisitLiteralExpr(ASTLiteral& expr) {
        switch (expr.type) {
            case Type::LIT_INT:    VM::vstack.push(MakeVar(VarType::I32 , static_cast<int32_t>(std::stoi(expr.literal.token)))); break;
            case Type::LIT_FLOAT:  VM::vstack.push(MakeVar(VarType::F64 , static_cast<double>(std::stod(expr.literal.token)))); break;
            case Type::LIT_CHAR:   VM::vstack.push(MakeVar(VarType::CHAR , expr.literal.token[0])); break;
            case Type::LIT_STRING: VM::vstack.push(MakeVar(VarType::STRING , expr.literal.token)); break;
            case Type::KW_I8:      VM::vstack.push(MakeVar(VarType::I8 , static_cast<int8_t>(std::stoi(expr.literal.token)))); break;
            case Type::KW_I16:     VM::vstack.push(MakeVar(VarType::I16 , static_cast<int16_t>(std::stoi(expr.literal.token)))); break;
            case Type::KW_I32:     VM::vstack.push(MakeVar(VarType::I32 , static_cast<int32_t>(std::stoi(expr.literal.token)))); break;
            case Type::KW_I64:     VM::vstack.push(MakeVar(VarType::I64 , static_cast<int64_t>(std::stoi(expr.literal.token)))); break;
            case Type::KW_U8:      VM::vstack.push(MakeVar(VarType::U8 , static_cast<uint8_t>(std::stoi(expr.literal.token)))); break;
            case Type::KW_U16:     VM::vstack.push(MakeVar(VarType::U16 , static_cast<uint16_t>(std::stoi(expr.literal.token)))); break;
            case Type::KW_U32:     VM::vstack.push(MakeVar(VarType::U32 , static_cast<uint32_t>(std::stoi(expr.literal.token)))); break;
            case Type::KW_U64:     VM::vstack.push(MakeVar(VarType::U64 , static_cast<uint64_t>(std::stoi(expr.literal.token)))); break;
            case Type::KW_F32:     VM::vstack.push(MakeVar(VarType::F32 , static_cast<float>(std::stof(expr.literal.token)))); break;
            case Type::KW_F64:     VM::vstack.push(MakeVar(VarType::F64 , static_cast<double>(std::stod(expr.literal.token)))); break;
            case Type::KW_BOOL:
                if (expr.literal.token == "true") {
                    VM::vstack.push(MakeVar(VarType::BOOL , true));
                } else if (expr.literal.token == "false") {
                    VM::vstack.push(MakeVar(VarType::BOOL , false));
                } else {
                    throw interpreter_error("Invalid boolean value");
                }
            break;
            case Type::KW_TRUE:   VM::vstack.push(MakeVar(VarType::BOOL , true)); break;
            case Type::KW_FALSE:  VM::vstack.push(MakeVar(VarType::BOOL , false)); break;
            case Type::KW_CHAR:   VM::vstack.push(MakeVar(VarType::CHAR , expr.literal.token[0])); break;
            case Type::KW_STRING: VM::vstack.push(MakeVar(VarType::STRING , expr.literal.token)); break;
            case Type::KW_VOID:   
                throw interpreter_error("Invalid use of void type");
            default:
                throw interpreter_error("Invalid Literal expression");
        }
    }


    void Interpreter::VisitLogicalExpr(ASTLogicalExpr& expr) {
        expr.lhs->Accept(*this);
        
        Variable lhs = VM::vstack.top();
        VM::vstack.pop();

        if (expr.op.type == Type::OP_LOGICAL_OR) {
            if (IsTruthy(lhs)) {
                VM::vstack.push(MakeVar(VarType::BOOL , true));
            } else {
                expr.rhs->Accept(*this);
                Variable rhs = VM::vstack.top();
                VM::vstack.pop();
                VM::vstack.push(MakeVar(VarType::BOOL , IsTruthy(rhs)));
            }
            return;
        } else {
            if (!IsTruthy(lhs)) {
                VM::vstack.push(MakeVar(VarType::BOOL , false));
            } else {
                expr.rhs->Accept(*this);
                Variable rhs = VM::vstack.top();
                VM::vstack.pop();
                VM::vstack.push(MakeVar(VarType::BOOL , IsTruthy(rhs)));
            }
            return;
        }
    }   

    void Interpreter::VisitUnaryExpr(ASTUnaryExpr& expr) {
        expr.expr->Accept(*this);

        Variable val = VM::vstack.top();
        VM::vstack.pop();

        switch (expr.op.type) {
            case Type::OP_SUB:
                switch (val.type) {
                    case VarType::I8:   VM::vstack.push(MakeVar(VarType::I8 , -std::get<int8_t>(val.value))); break;
                    case VarType::I16:  VM::vstack.push(MakeVar(VarType::I16 , -std::get<int16_t>(val.value))); break;
                    case VarType::I32:  VM::vstack.push(MakeVar(VarType::I32 , -std::get<int32_t>(val.value))); break;
                    case VarType::I64:  VM::vstack.push(MakeVar(VarType::I64 , -std::get<int64_t>(val.value))); break;
                    case VarType::F32:  VM::vstack.push(MakeVar(VarType::F32 , -std::get<float>(val.value))); break;
                    case VarType::F64:  VM::vstack.push(MakeVar(VarType::F64 , -std::get<double>(val.value))); break;
                    case VarType::U8:   [[fallthrough]];
                    case VarType::U16:  [[fallthrough]];
                    case VarType::U32:  [[fallthrough]];
                    case VarType::U64:
                        throw interpreter_error("Invalid unary operator applied to unsigned integer");
                        
                    default:
                        throw interpreter_error("Unary operator applied to Invalid type");
                }
            break;
            case Type::OP_EXCLAMATION: VM::vstack.push(MakeVar(VarType::BOOL , !IsTruthy(val))); break;
            default:
                throw interpreter_error("Invalid unary operator");
        }
    }

    void Interpreter::VisitBinaryExpr(ASTBinaryExpr& expr) {
        expr.rhs->Accept(*this);
        expr.lhs->Accept(*this);

        Variable lhs = VM::vstack.top();
        VM::vstack.pop();

        Variable rhs = VM::vstack.top();
        VM::vstack.pop();
        
        EvaluateBinaryExpr(expr.op.type , lhs , rhs);
    }

    void Interpreter::VisitCallExpr(ASTCallExpr& expr) {
        call_expr = true;
        expr.callee->Accept(*this);

        Variable callee = VM::vstack.top();
        VM::vstack.pop();

        // we've pushed the function identifier to stack and retrieved it
        //      so we set this to false
        call_expr = false;

        if (callee.type != VarType::CALLABLE)
            throw interpreter_error("Cannot call non callable identifier");

        std::vector<Variable> args{};
        for (auto& arg : expr.args) {
            arg->Accept(*this);
            args.push_back(VM::vstack.top());
            VM::vstack.pop();
        }

        Callable* func = VM::env->GetCallable(std::get<std::string>(callee.value));

        if (func == nullptr) 
            throw interpreter_error("Undefined function " + std::get<std::string>(callee.value));
        
        Variable ret = func->Call(*this , args , false);
        VM::vstack.push(ret);
    }

    void Interpreter::VisitGroupingExpr(ASTGroupingExpr& expr) {
        expr.expr->Accept(*this);
    }

    void Interpreter::VisitVarExpr(ASTVarExpr& expr) {
        if (!call_expr) {
            if (VM::env->GetVar(expr.name.token) == nullptr)
            throw interpreter_error(
                expr.name.line , expr.name.column ,
                "Undefined variable " + expr.name.token
            );
            VM::vstack.push(*VM::env->GetVar(expr.name.token));       
        } else {
            if (VM::env->GetVar(expr.name.token) != nullptr) { 
                throw interpreter_error(
                    expr.name.line , expr.name.column ,
                    expr.name.token + " is not callable"
                );
            } else {
                VM::vstack.push(Variable{ VarType::CALLABLE , expr.name.token });
            }
        }
    }
    
    /// \todo Fix this, right now it only searches the current scope
    ///         will fail if the variable is not in the current scope
    /// \note Need to change variable in place in environment it already
    void Interpreter::VisitVarAssignExpr(ASTVarAssignExpr& expr) {
        expr.assignment->Accept(*this);
        Variable val = VM::vstack.top();
        VM::vstack.pop();
        if (VM::env->GetVar(expr.name.token) == nullptr)
            throw interpreter_error(
                expr.name.line , expr.name.column ,
                "Undefined variable " + expr.name.token + " in assignment"
            );

        Variable* var = GetVar(expr.name.token);

        if (static_cast<uint32_t>(var->type) <= 9 && static_cast<uint32_t>(val.type) > 9)
            throw interpreter_error(
                expr.name.line , expr.name.column ,
                "Invalid assignment of type " + std::to_string(static_cast<uint8_t>(val.type)) + " to variable of type " + std::to_string(static_cast<uint8_t>(var->type))
            );
        
        if (static_cast<uint32_t>(var->type) > 9 && static_cast<uint32_t>(val.type) <= 9)
            throw interpreter_error(
                expr.name.line , expr.name.column ,
                "Invalid assignment of type " + std::to_string(static_cast<uint8_t>(val.type)) + " to variable of type " + std::to_string(static_cast<uint8_t>(var->type))
            );

        if (static_cast<uint32_t>(var->type) == 12 && static_cast<uint32_t>(val.type) <= 10) {
            throw interpreter_error(
                expr.name.line , expr.name.column ,
                "Invalid assignment of type " + std::to_string(static_cast<uint8_t>(val.type)) + " to variable of type " + std::to_string(static_cast<uint8_t>(var->type))
            );
        }

        *var = val;
    }
    
    void Interpreter::VisitPrintStmnt(ASTPrintStmnt& stmnt) {
        stmnt.expr->Accept(*this);

        Variable val = VM::vstack.top();
        VM::vstack.pop();
        
        switch (val.type) {
            case VarType::I8:   std::cout << std::get<int8_t>(val.value) << "\n"; break;
            case VarType::I16:  std::cout << std::get<int16_t>(val.value) << "\n"; break;
            case VarType::I32:  std::cout << std::get<int32_t>(val.value) << "\n"; break;
            case VarType::I64:  std::cout << std::get<int64_t>(val.value) << "\n"; break;
            case VarType::U8:   std::cout << std::get<uint8_t>(val.value) << "\n"; break;
            case VarType::U16:  std::cout << std::get<uint16_t>(val.value) << "\n"; break;
            case VarType::U32:  std::cout << std::get<uint32_t>(val.value) << "\n"; break;
            case VarType::U64:  std::cout << std::get<uint64_t>(val.value) << "\n"; break;
            case VarType::F32:  std::cout << std::get<float>(val.value) << "\n"; break;
            case VarType::F64:  std::cout << std::get<double>(val.value) << "\n"; break;
            case VarType::BOOL: std::cout << std::get<bool>(val.value) << "\n"; break;
            case VarType::CHAR: std::cout << std::get<char>(val.value) << "\n"; break;
            case VarType::STRING: std::cout << std::get<std::string>(val.value) << "\n"; break;
            default:
                throw interpreter_error("Invalid type for print statement");
        }
    }

    void Interpreter::VisitExprStmnt(ASTExprStmnt& stmnt) {
        stmnt.expr->Accept(*this);
    }   

    void Interpreter::VisitVarStmnt(ASTVarStmnt& stmnt) {
        if (VM::env->GetVarFromScope(stmnt.name.token) != nullptr)
            throw interpreter_error("Redefenition of variable " + stmnt.name.token);
        if (stmnt.initializer == nullptr)
            throw interpreter_error("Variable " + stmnt.name.token + " must be initialized");

        stmnt.initializer->Accept(*this);
        Variable var = VM::vstack.top();
        VM::vstack.pop();
        VM::env->AddVar(stmnt.name.token , var);
    }
    
    void Interpreter::VisitBlockStmnt(ASTBlockStmnt& stmnt) {
        VM::env->PushScope();
        for (auto& stmnt : stmnt.statements) {
            try {
                stmnt->Accept(*this);
            } catch (const interpreter_error& e) {
                VM::env->PopScope();
                throw e;
            }
        }
        VM::env->PopScope();
    }

    void Interpreter::VisitIfStmnt(ASTIfStmnt& stmnt) {
        stmnt.condition->Accept(*this);
        Variable val = VM::vstack.top();
        VM::vstack.pop();
        if (IsTruthy(val)) {
            stmnt.then_branch->Accept(*this);
        } else if (stmnt.else_branch != nullptr) {
            stmnt.else_branch->Accept(*this);
        }
    }
    
    void Interpreter::VisitWhileStmnt(ASTWhileStmnt& stmnt) {
        stmnt.condition->Accept(*this);

        Variable val = VM::vstack.top();
        VM::vstack.pop();

        while (IsTruthy(val)) {
            stmnt.body->Accept(*this);
            stmnt.condition->Accept(*this);
            val = VM::vstack.top();
            VM::vstack.pop();
        }
    }
    
    void Interpreter::VisitFunction(ASTFunction& stmnt) {
        std::shared_ptr<Callable> f = std::make_shared<Function>(stmnt.body , VM::env , stmnt.params);
        VM::env->AddFunction(stmnt.name.token , f);
    }
    
    void Interpreter::VisitReturnStmnt(ASTReturnStmnt& stmnt) {
        if (stmnt.expr == nullptr) {
            throw Return(Variable{});
        } else {
            stmnt.expr->Accept(*this);
            Variable val = VM::vstack.top();
            VM::vstack.pop();
            throw Return(val);
        }
    }
    
    void Interpreter::ExecuteBlock(std::shared_ptr<ASTStmnt> body , std::shared_ptr<Environment> closure){
        std::shared_ptr<Environment> prev = VM::env;
        VM::env = closure;
        body->Accept(*this);
        VM::env = prev;
    }

}