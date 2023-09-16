/**
 * \file errors.hpp
 * \author Y
 * \version 0.0.1
 * \brief contains major error handling code and classes
 * \details includes the error handler class which has support for lexer, parser,
 *              interpreter, and compiler errors and also contains the definitions for different 
 *              error support classes such as lexer_error, parser_error, interpreter_error, and compiler_error
 *              which are used to throw errors with line and column information
*/

#ifndef YLANG_ERRORS_HPP
#define YLANG_ERRORS_HPP

#include <string>
#include <stdexcept>
#include <stack>
#include <mutex>

namespace ylang {

    /// \enum ErrorLevel
    enum class ErrorLevel {
        WARNING ,
        FATAL ,
        UNDEFINED_BEHAVIOR
    };

    /// \class lexer_error
    /// \brief Class used to throw lexer errors with line and column information with
    ///        help to specify which part of the compiler the error message is coming from
    class lexer_error : public std::runtime_error {
        public:

            ErrorLevel level = ErrorLevel::UNDEFINED_BEHAVIOR;

            uint32_t line = 0;
            uint32_t column = 0;
            
            /// \param msg the error message
            /// \note This constructor is used for debugging purposes only and helps determine
            ///       if the program is behaving as expected
            lexer_error(const std::string& msg) : std::runtime_error("[YLexerError] " + msg) {}

            /// \param line the line number of the error
            /// \param col the column number of the error
            /// \param msg the error message
            /// \note This constructor is used for debugging purposes only and helps determine
            ///       if the program is behaving as expected
            lexer_error(uint32_t line , uint32_t col , const std::string& msg) :
                line(line) , column(col) ,
                std::runtime_error("[YLexerError] " + msg + " ( " + std::to_string(line) + ":" + std::to_string(col) + " )") {}

            /// \param level the level of the error
            /// \param line the line number of the error
            /// \param col the column number of the error
            /// \param msg the error message
            lexer_error(ErrorLevel level , uint32_t line , uint32_t col , const std::string& msg) :
                level(level) , line(line) , column(col) ,
                std::runtime_error("[YLexerError] " + msg + " ( " + std::to_string(line) + ":" + std::to_string(col) + " )") {}
    };

    /// \class parser_error
    /// \brief Class used to throw parser errors with line and column information with 
    ///        help to specify which part of the compiler the error message is coming from
    class parser_error : public std::runtime_error {
        public:
        
            ErrorLevel level = ErrorLevel::UNDEFINED_BEHAVIOR;

            uint32_t line = 0;
            uint32_t column = 0;
            
            /// \param msg the error message
            /// \note This constructor is used for debugging purposes only and helps determine
            ///       if the program is behaving as expected
            parser_error(const std::string& msg) : std::runtime_error("[YParserError] " + msg) {}
            
            /// \param line the line number of the error
            /// \param col the column number of the error
            /// \param msg the error message
            /// \note This constructor is used for debugging purposes only and helps determine
            ///       if the program is behaving as expected
            parser_error(uint32_t line , uint32_t col , const std::string& msg) :
                line(line) , column(col) ,
                std::runtime_error("[YParserError] " + msg + " ( " + std::to_string(line) + ":" + std::to_string(col) + " )") {}

            /// \param level the level of the error
            /// \param line the line number of the error
            /// \param col the column number of the error
            /// \param msg the error message
            parser_error(ErrorLevel level , uint32_t line , uint32_t col , const std::string& msg) :
                level(level) , line(line) , column(col) ,
                std::runtime_error("[YParserError] " + msg + " ( " + std::to_string(line) + ":" + std::to_string(col) + " )") {}
    };

    /// \class interpreter_error
    /// \brief Class used to throw compiler errors with line and column information with
    ///        help to specify which part of the compiler the error message is coming from
    class interpreter_error : public std::runtime_error {
        public:
        
            ErrorLevel level = ErrorLevel::UNDEFINED_BEHAVIOR;
        
            uint32_t line = 0;
            uint32_t column = 0;
            
            /// \param msg the error message
            /// \note This constructor is used for debugging purposes only and helps determine
            ///       if the program is behaving as expected
            interpreter_error(const std::string& msg) : std::runtime_error("[YInterpreterError] " + msg) {}

            /// \param line the line number of the error
            /// \param col the column number of the error
            /// \param msg the error message
            /// \note This constructor is used for debugging purposes only and helps determine
            ///       if the program is behaving as expected
            interpreter_error(uint32_t line , uint32_t col , const std::string& msg) :
                line(line) , column(col) ,
                std::runtime_error("[YInterpreterError] " + msg + " ( " + std::to_string(line) + ":" + std::to_string(col) + " )") {}

            /// \param level the level of the error
            /// \param line the line number of the error
            /// \param col the column number of the error
            /// \param msg the error message
            interpreter_error(ErrorLevel level , uint32_t line , uint32_t col , const std::string& msg) :
                level(level) , line(line) , column(col) ,
                std::runtime_error("[YInterpreterError] " + msg + " ( " + std::to_string(line) + ":" + std::to_string(col) + " )") {}
    };

    /// \class compiler_error
    /// \brief Class used to throw compiler errors with line and column information with
    ///        help to specify which part of the compiler the error message is coming from
    class compiler_error : public std::runtime_error {
        public:
        
            ErrorLevel level = ErrorLevel::UNDEFINED_BEHAVIOR;
        
            uint32_t line = 0;
            uint32_t column = 0;
            
            /// \param msg the error message
            /// \note This constructor is used for debugging purposes only and helps determine
            ///       if the program is behaving as expected
            compiler_error(const std::string& msg) : std::runtime_error("[YCompilerError] " + msg) {}

            /// \param line the line number of the error
            /// \param col the column number of the error
            /// \param msg the error message
            /// \note This constructor is used for debugging purposes only and helps determine
            ///       if the program is behaving as expected
            compiler_error(uint32_t line , uint32_t col , const std::string& msg) :
                line(line) , column(col) ,
                std::runtime_error("[YCompilerError] " + msg + " ( " + std::to_string(line) + ":" + std::to_string(col) + " )") {}

            /// \param level the level of the error
            /// \param line the line number of the error
            /// \param col the column number of the error
            /// \param msg the error message
            compiler_error(ErrorLevel level , uint32_t line , uint32_t col , const std::string& msg) :
                level(level) , line(line) , column(col) ,
                std::runtime_error("[YCompilerError] " + msg + " ( " + std::to_string(line) + ":" + std::to_string(col) + " )") {}
    };

    /// \struct Error
    /// \brief Struct used to store errors in the error handler
    struct Error {

        /// \note we initialize the level to undefined behavior for the sake of safety
        ErrorLevel level = ErrorLevel::UNDEFINED_BEHAVIOR;

        uint32_t line = 0;
        uint32_t column = 0;
        std::string msg = "";

        /// \note This constructor is used for debugging purposes only and helps determine
        ///       if the program is behaving as expected
        Error() : msg("< UNDEFINED BEHAVIOR >") {}

        /// \param msg the error message
        /// \note This constructor is used for debugging purposes only and helps determine
        ///       if the program is behaving as expected
        Error(const std::string& msg) : msg(msg + "< error location unknown >") {}

        /// \param line the line number of the error
        /// \param col the column number of the error
        /// \param msg the error message
        /// \note This constructor is the recommended constructor to use when throwing errors
        Error(uint32_t line, uint32_t col, const std::string& msg) :
            level(ErrorLevel::FATAL) , line(line) , column(col) ,
            msg(msg + " ( " + std::to_string(line) + ":" + std::to_string(col) + " )") {}

        /// \param level the level of the error
        /// \param line the line number of the error
        /// \param column the column number of the error
        /// \param msg the error message
        Error(ErrorLevel level, uint32_t line, uint32_t col, const std::string& msg) :
            level(level) , line(line) , column(col) ,
            msg(msg + " ( " + std::to_string(line) + ":" + std::to_string(col) + " )") {}

        /// \param error the ylang lexer error
        Error(const lexer_error& error);

        /// \param error the ylang parser error
        Error(const parser_error& error);

        /// \param error the ylang interpreter error
        Error(const interpreter_error& error);

        /// \param error the ylang compiler error
        Error(const compiler_error& error);
    };

    /// \class ErrorHandler
    /// \brief Class used to handle errors
    class ErrorHandler {

        std::stack<Error> error_queue;

        std::string FormatLexerError(const Error& error , const std::string& src , uint32_t line_strt_index , uint32_t index);
        std::string FormatError(const Error& err);

        public:

            ErrorHandler() {}
            ~ErrorHandler() {}

            /// \brief Add an error to the error queue
            /// \param error the error to add
            inline void SubmitError(const Error& error) { error_queue.push(error); }

            /// \brief Check if the error queue is empty
            /// \return bool : true if the error queue is empty, false otherwise
            inline bool IsEmpty() const { return error_queue.empty(); }

            /// \brief Flush the error queue if parse was aborted
            void FlushErrors();
            void ClearErrors();
            
            /// \brief Formats and prints lexer error
            void HandleLexerError(const Error& error , const std::string& src , uint32_t line_strt_index , uint32_t index); 

            /// \brief Formats and prints parser error
            void HandleParserError(const Error& error);

            /// \brief Formats and prints interpreter error
            void HandleRuntimeError(const Error& error);
    };



}

#endif // !YLANG_ERRORS_HPP