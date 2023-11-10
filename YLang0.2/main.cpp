#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>

#include "errors.hpp"
#include "util.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "ast.hpp"
#include "interpreter.hpp"

namespace ylang {

    int Main(const std::string& src) {

        std::shared_ptr<ylang::ErrorHandler> err_handler = std::make_shared<ylang::ErrorHandler>();
        ylang::Lexer lexer(err_handler , src);

        try {
            lexer.Lex();    
        } catch (std::runtime_error& e) {
            std::cerr << "< Error > :: " << e.what() << std::endl;
            return 1;
        }
        
        std::vector<ylang::Token> lexemes = lexer.GetTokens();

#if 0
        for (auto& token : lexemes)
            ylang::Util::PrintToken(token);
#endif
#if 1
        ylang::Parser parser(err_handler , lexemes);
        std::vector<std::shared_ptr<ASTStmnt>> stmnts{};
        try {
            stmnts = parser.Parse();
        } catch (ylang::parser_error& e) {
            err_handler->HandleParserError(e);
        }

        if (parser.ParseFailed()) {
            std::cerr << "< Error > :: Unable to parse file" << std::endl;
            err_handler->FlushErrors();
            return 1;
        } else if (!err_handler->IsEmpty()) {
            err_handler->FlushErrors();
        }
#endif
#if 1
        std::cout << "\n";
        std::shared_ptr<ylang::ExprVisitor> printer = std::make_shared<ylang::ASTPrinter>();
        for (auto& stmnt : stmnts)
            stmnt->Accept(*(printer.get()));
        std::cout << "\n\n";
#endif
#if 1
        VM::Interpret(stmnts);
#endif
        std::cout.flush();
        return 0;
    }

} 

int main(int argc , char* argv[]) {
    try {
        std::string src;
        ylang::Util::ReadCmndLineArgs(&argc , argv);
        src = ylang::Util::ReadSrc();
        return ylang::Main(src);
    } catch (std::runtime_error& e) {
        std::cerr << "\n" << e.what() << std::endl;
        ylang::Util::PrintUsage();
        return 1;
    }

    throw std::runtime_error("Unreachable code");
}