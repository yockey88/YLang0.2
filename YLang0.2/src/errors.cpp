#include "errors.hpp"

#include <iostream>

namespace ylang {

    Error::Error(const lexer_error& error) {
        this->level = error.level;
        this->line = error.line;
        this->column = error.column;
        this->msg = error.what();
    }

    Error::Error(const parser_error& error) {
        this->level = error.level;
        this->line = error.line;
        this->column = error.column;
        this->msg = error.what();
    }

    Error::Error(const interpreter_error& error) {
        this->level = error.level;
        this->line = error.line;
        this->column = error.column;
        this->msg = error.what();
    }

    Error::Error(const compiler_error& error) {
        this->level = error.level;
        this->line = error.line;
        this->column = error.column;
        this->msg = error.what();
    }

    std::string ErrorHandler::FormatLexerError(const Error& error , const std::string& src , uint32_t line_strt_index , uint32_t index) {
        std::string formatted_error = "\n\t>";
        size_t end_line_index = src.find('\n' , line_strt_index);
        size_t line_length = end_line_index - line_strt_index;
        formatted_error += src.substr(line_strt_index , line_length) + "\n\t";
        
        int i = 0;
        for (; i < error.column - 1; ++i)
            formatted_error += '~';
        
        formatted_error += '^';

        for (; i < line_length; ++i)
            formatted_error += '~';
        
        formatted_error += "\n";

        return formatted_error;
    }

    std::string ErrorHandler::FormatError(const Error& err) {
        std::string formatted_error = "\n\t>   " + err.msg;
        return formatted_error;
    }

    void ErrorHandler::FlushErrors() {
        if (error_queue.empty()) return;
        while (!error_queue.empty()) {
            Error err = error_queue.top();
            error_queue.pop();
            HandleParserError(err);
        }
    }

    void ErrorHandler::ClearErrors() {
        if (error_queue.empty()) return;
        while (!error_queue.empty()) error_queue.pop();
    }

    void ErrorHandler::HandleLexerError(const Error& error , const std::string& src , uint32_t line_strt_index , uint32_t index) {
        std::cout << "\nErrors Encountered | Flushing Errors" << std::endl;
        switch (error.level) {
            case ErrorLevel::WARNING:
                std::cout << "< Warning > :: \n\t";
                std::cout << error.msg << "\n";
                std::cout << FormatLexerError(error , src , line_strt_index , index) << std::endl;
            break;
            case ErrorLevel::FATAL:
                std::cout << "< Fatal > :: \n\t";
                std::cout << error.msg << "\n";
                std::cout << FormatLexerError(error , src , line_strt_index , index) << std::endl;
            break;
            default:
                std::cout << "< UNDEFINED BEHAVIOR > :: \n\t";
                std::cout << error.msg << "\n";
                std::cout << FormatLexerError(error , src , line_strt_index , index) << std::endl;
            break;
        }
    }
    
    void ErrorHandler::HandleParserError(const Error& error) {
        std::cout << "\nErrors Encountered | Flushing Errors" << std::endl;
        switch (error.level) {
            case ErrorLevel::WARNING:
                std::cout << "< Warning > :: \n\t";
                std::cout << FormatError(error) << std::endl;
            break;
            case ErrorLevel::FATAL:
                std::cout << "< Fatal > :: \n\t";
                std::cout << FormatError(error) << std::endl;
            break;
            default:
                std::cout << "< UNDEFINED BEHAVIOR > :: \n\t";
                std::cout << FormatError(error) << std::endl;
            break;
        }
    }

    void ErrorHandler::HandleRuntimeError(const Error& error) {
        std::cout << "\nErrors Encountered | Flushing Errors" << std::endl;
        switch (error.level) {
            case ErrorLevel::WARNING:
                std::cout << "< Warning > :: \n\t";
                std::cout << FormatError(error) << std::endl;
            break;
            case ErrorLevel::FATAL:
                std::cout << "< Fatal > :: \n\t";
                std::cout << FormatError(error) << std::endl;
            break;
            default:
                std::cout << "< UNDEFINED BEHAVIOR > :: \n\t";
                std::cout << FormatError(error) << std::endl;
            break;
        }
        /// \todo spit out stack trace
    }

}