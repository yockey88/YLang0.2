#include "util.hpp"

#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <filesystem>

namespace ylang {

    std::vector<std::string> CmndLineArgs::flags = {
        "-h" , " --help" ,      
        "-v" , " --version" ,      
        "-d" , " --debug" ,          
        "-D" , " --dev" ,
        "-o" , 
        "-t" , "--time" ,
        "-l" ,
        "-a" , 
        "-S" , "--asm-only" ,
        "-O" , "--optimize" ,
    };
    
    char CmndLineArgs::GetFlagFromString(const std::string& flag) const {

        if (flag.substr(2) == "help") return 'h';
        if (flag.substr(2) == "version") return 'v';
        if (flag.substr(2) == "debug") return 'd';
        if (flag.substr(2) == "dev") return 'D';
        if (flag.substr(2) == "time") return 't';
        if (flag.substr(2) == "asm-only") return 'S';
        if (flag.substr(2) == "optimize") return 'O';
        return 'x';

    }

    CmndLineArgs::CmndLineArgs(int *& argc , char* argv[]) {

        args['y'] = argv[0];

        bool flags_valid = true;
        std::string invalid_flag = "";
        std::string err_msg = "";

        for (int i = 1 ; i < (*argc) ; ++i) {
            std::string raw_arg{ argv[i] };

            std::string flag = "";
            std::optional<std::string> flag_arg = std::nullopt;

            // check if argument is short flag and not just two dashes
            if (raw_arg.length() == 2 && raw_arg[0] == '-' && raw_arg[1] != '-') {
                flag = raw_arg.substr(1);
            
            // check if argument is just two dashes
            } else if (raw_arg.length() == 2 && raw_arg[0] == '-' && raw_arg[1] == '-') {
                flags_valid = false;
                invalid_flag = raw_arg;
                err_msg = "< Error :: Invalid flag( " + invalid_flag + " )>";
                break;

            // check if argument is file path (should be only argument with a '.' in it)
            // if it is longer than 2 characters and has a '.' in it, it is a file path
            //      or if not it will be caught when trying to open the file
            } else if (raw_arg.length() > 2 && raw_arg.find('.') != std::string::npos) {
                args['f'] = raw_arg;
                continue;

            // check if argument is long flag and retrieve short flag version
            } else if (raw_arg.length() > 2 && raw_arg[0] == '-' && raw_arg[1] == '-') {
                flag += GetFlagFromString(raw_arg.substr(2));
            }

            // once the flag is processed see if it requires a value to go with it
            if (FlagRequiresValue(flag[0]) && i < (*argc) - 1 && argv[i + 1][0] != '-') {
                flag_arg = argv[i + 1];
                ++i;
            } else if ((FlagRequiresValue(flag[0]) && i == (*argc) - 1) || 
                       (FlagRequiresValue(flag[0] && i < (*argc) - 1 && argv[i + 1][0] == '-'))) {

                flags_valid = false;
                invalid_flag = flag;
                err_msg = "< Error :: Flag( " + invalid_flag + " ) requires a value >";
                break;
            }
            
            args[flag[0]] = flag_arg;
        }

        // check we found a file since it is required
        if (args.find('f') == args.end()) {
            flags_valid = false;
            err_msg = "< Error :: No file path provided >";
        }
        
        if (!flags_valid) throw std::runtime_error(err_msg);
    }
    
    std::unique_ptr<CmndLineArgs> Util::cmnd_line_args = nullptr;

    /// \brief See Y Lang documentation for more information 
    const void Util::PrintUsage() {
        std::cout << "\n< Usage > :: [ ylc [options] -f <file-path> ]";
        std::cout << "\n< Options > ::\n\n";
        std::cout << "  -f <file-path>               Passed in the specified file\n";
        std::cout << "  -i, --interpret              Interprets instead of compilers the output\n";
        std::cout << "  -h, --help                   Prints this message\n";
        std::cout << "  -v, --version                Prints the version\n";
        std::cout << "  -d, --debug                  Prints debug information\n";
        std::cout << "  -D, --dev                    Enables all development debug information\n";
        std::cout << "  -o <file-path>               <file path>\tWrites the output to the specified file\n";
        std::cout << "  -t, --time                   Prints the time taken to compile and run the output\n";
        std::cout << "  -l <file path>               Writes the llvm ir to the specified file\n";
        std::cout << "  -a <file path>               Writes the assembly code to the specified file\n";
        std::cout << "  -S <file-path>               Writes the assembly code to the specified file and does not compile\n";
        std::cout << "  -O <level>, --optimize       Optimizes the output with the specified optimization level (--optimize uses -03)\n";
        std::cout << "  -C, --comments               Auto generates documentation based on comments\n";
     // std::cout << "  -s, --silent                 Silences the output\n";
     // std::cout << "  -r, --run                    Runs the output\n";
        std::cout << std::endl;
    }

    void Util::ReadCmndLineArgs(int* argc , char* argv[]) {
        if ((*argc) < 2) 
            throw std::runtime_error("< Error :: No file path provided >");

        try {
            cmnd_line_args = std::make_unique<CmndLineArgs>(argc , argv);
        } catch (const std::runtime_error& e) {
            throw e;
        }
    }

    std::string Util::ReadSrc() {

        std::string file_path = Util::GetCmndLineArgs()->GetArgValue('f');
        std::filesystem::path path(file_path);

        if (file_path.substr(file_path.find('.')).length() > 4 || file_path.substr(file_path.find('.')).length() < 1)
            throw std::runtime_error("< Error :: Invalid file extension( " + file_path.substr(file_path.find('.')) + " )>");

        if (!std::filesystem::exists(path))
            throw std::runtime_error("< Error :: File does not exist( " + file_path + " )>");

        if (std::filesystem::is_directory(path))
            throw std::runtime_error("< Error :: Given path is a directory( " + file_path + " )>");
        
        std::string ext = file_path.substr(file_path.find('.'));

        if (ext != ".yl" && ext != ".yproj" && ext != ".yobj" && ext != ".txt")
            throw std::runtime_error("< Error :: Invalid file extension( " + file_path.substr(file_path.find('.')) + " )>");

        std::ifstream file(file_path);

        if (!file.is_open())
            throw std::runtime_error("< Error :: Could not open file( " + file_path + " )>");
        
        std::streampos p = file.tellg();
        std::stringstream buffer;

        buffer << file.rdbuf(); 
        file.seekg(p);

        return buffer.str();
    }

    void Util::PrintToken(const Token& token) {
        std::cout << "< " << TokenTypeStrings[static_cast<int>(token.type)] 
                  << " ( " << token.line << " , " << token.column << " )> :: " 
                  << token.token << std::endl;
    }
    
    const std::string Util::GetCurrFileName() {
        try {
            return (cmnd_line_args->ArgPresent('f')) ?
                    cmnd_line_args->GetArgValue('f') :
                    "<no-file.yl>";
        } catch(...) {
            DEBUG_PRINT("Catching no file name error, returning <no-file.yl>");
            return "<no-file.yl>";
        }
    }

}
