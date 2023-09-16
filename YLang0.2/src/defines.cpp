#include "defines.hpp"

#include <iostream>

#include "errors.hpp"

namespace ylang {

    VarValue operator+(const VarValue& lhs , const VarValue& rhs) {
        if (((lhs.index() >= 0 && lhs.index() <= 9) || (lhs.index() >= 11 && lhs.index() <= 12)) &&
            ((rhs.index() >= 0 && rhs.index() <= 12) || (rhs.index() >= 11 && rhs.index() <= 12))) {

            auto ret = std::visit([](auto&& arg1 , auto&& arg2) -> VarValue {
                return arg1 + arg2;
            } , lhs , rhs);    

            return ret;
        }

        throw interpreter_error("Invalid binary operator applied to operands");
    }

    VarValue operator-(const VarValue& lhs , const VarValue& rhs) {
       if ((lhs.index() >= 0 && lhs.index() <= 9) && (rhs.index() >= 0 && rhs.index() <= 9)) {

            return std::visit([](auto&& arg1 , auto&& arg2) -> VarValue {
                return arg1 - arg2;
            } , lhs , rhs);
        }

        throw interpreter_error("Invalid binary operator applied to operands");
    }

    VarValue operator*(const VarValue& lhs , const VarValue& rhs) {
        if ((lhs.index() >= 0 && lhs.index() <= 9) && (rhs.index() >= 0 && rhs.index() <= 9)) {

            return std::visit([](auto&& arg1 , auto&& arg2) -> VarValue {
                return arg1 * arg2;
            } , lhs , rhs);
        }
        
        throw interpreter_error("Invalid binary operator applied to operands");
    }

    VarValue operator/(const VarValue& lhs , const VarValue& rhs) {
        if ((lhs.index() >= 0 && lhs.index() <= 9) && (rhs.index() >= 0 && rhs.index() <= 9)) {

            return std::visit([](auto&& arg1 , auto&& arg2) -> VarValue {
                return arg1 / arg2;
            } , lhs , rhs);
        }

        throw interpreter_error("Invalid binary operator applied to operands");
    }

    VarValue operator%(const VarValue& lhs , const VarValue& rhs) {
        throw interpreter_error("Binary operator 'modulo' unimplemented");
    }
    
    bool operator==(const VarValue& lhs , const VarValue& rhs) {
        if (lhs.index() != rhs.index()) {
            return false;
        }

        return std::visit([](auto&& arg1 , auto&& arg2) -> bool {
            return arg1 == arg2;
        } , lhs , rhs);
    }

    bool operator!=(const VarValue& lhs , const VarValue& rhs) {
        if (lhs.index() != rhs.index()) {
            return true;
        }

        return std::visit([](auto&& arg1 , auto&& arg2) -> bool {
            return arg1 != arg2;
        } , lhs , rhs);
    }

    bool operator<(const VarValue& lhs , const VarValue& rhs) {
        if (lhs.index() != rhs.index()) {
            throw interpreter_error("Invalid binary operator applied to operands");
        }

        return std::visit([](auto&& arg1 , auto&& arg2) -> bool {
            return arg1 < arg2;
        } , lhs , rhs);
    }

    bool operator<=(const VarValue& lhs , const VarValue& rhs) {
        if (lhs.index() != rhs.index()) {
            throw interpreter_error("Invalid binary operator applied to operands");
        }

        return std::visit([](auto&& arg1 , auto&& arg2) -> bool {
            return arg1 <= arg2;
        } , lhs , rhs);
    }

    bool operator>(const VarValue& lhs , const VarValue& rhs) {
        if (lhs.index() != rhs.index()) {
            throw interpreter_error("Invalid binary operator applied to operands");
        }

        return std::visit([](auto&& arg1 , auto&& arg2) -> bool {
            return arg1 > arg2;
        } , lhs , rhs);
    }

    bool operator>=(const VarValue& lhs , const VarValue& rhs) {
        if (lhs.index() != rhs.index()) {
            throw interpreter_error("Invalid binary operator applied to operands");
        }

        return std::visit([](auto&& arg1 , auto&& arg2) -> bool {
            return arg1 >= arg2;
        } , lhs , rhs);
    }
    
    void operator<<(std::ostream& os , const VarValue& value) {
        std::visit([&os](auto&& arg) {
            os << arg;
        } , value);
    }

    void operator<<(std::ostream& os , const Variable& var) {
        os << VarTypeStrings[static_cast<uint32_t>(var.type)] << " : " << var.value;
    }
    
    VarType VarTypeFromToken(Type type) {
        switch (type) {
            case Type::LIT_INT: return VarType::I32;
            case Type::LIT_FLOAT: return VarType::F64;
            case Type::LIT_CHAR: return VarType::CHAR;
            case Type::LIT_STRING: return VarType::STRING;
            case Type::KW_TRUE: return VarType::BOOL;
            case Type::KW_FALSE: return VarType::BOOL;
            case Type::KW_I8: return VarType::I8;
            case Type::KW_I16: return VarType::I16;
            case Type::KW_I32: return VarType::I32;
            case Type::KW_I64: return VarType::I64;
            case Type::KW_U8: return VarType::U8;
            case Type::KW_U16: return VarType::U16;
            case Type::KW_U32: return VarType::U32;
            case Type::KW_U64: return VarType::U64;
            case Type::KW_F32: return VarType::F32;
            case Type::KW_F64: return VarType::F64;
            case Type::KW_BOOL: return VarType::BOOL;
            case Type::KW_CHAR: return VarType::CHAR;
            case Type::KW_STRING: return VarType::STRING;
            case Type::KW_BYTE: return VarType::BYTE;
            case Type::KW_NULL: return VarType::NULL_T;
            case Type::KW_VOID: return VarType::VOID;
            case Type::KW_STRUCT: return VarType::STRUCT;
            case Type::KW_CLASS: return VarType::CLASS;
            case Type::KW_OBJECT: return VarType::OBJECT;
            case Type::KW_ENUM: return VarType::ENUM;
            case Type::KW_UNION: return VarType::UNION;
            default:
                throw interpreter_error("Invalid type");
        }
    }

}