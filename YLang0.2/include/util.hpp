/**
 * \file util.hpp
 * \author Y
 * \version 0.0.1
 * \brief contains miscallaneous utility functions 
 * \details the utilities here are more core to the infrastructure of the language 
 *              than the ones in defines.hpp
*/

#ifndef YLANG_UTIL_HPP
#define YLANG_UTIL_HPP

#include <string>
#include <map>
#include <optional>
#include <memory>
#include <vector>
#include <iostream>

#include "defines.hpp"

namespace ylang {

    /// \class CmndLineArgs
    /// \brief used to parse the command line arguments
    class CmndLineArgs {

        /// \brief list of all the flags
        static std::vector<std::string> flags;

        /// \brief list of all the command line arguments
        std::map<char , std::optional<std::string>> args{};

        char GetFlagFromString(const std::string& flag) const;

        inline const bool FlagRequiresValue(char flag) const { return (flag == 'o' || flag == 'l' || flag == 'a' || flag == 'S' || flag == 'O') ? true : false; }

        friend class Util;

        public:

            /// \brief parses the command line arguments
            /// \param argc number of command line arguments
            /// \param argv list of command line arguments
            CmndLineArgs(int *& argc , char* argv[]);

            /// \note we delete these to prevent copying and moving and ensure we only have one
            ///         instance of our arugments and control data  
            CmndLineArgs() = delete;
            CmndLineArgs(const CmndLineArgs&) = delete;
            CmndLineArgs(CmndLineArgs&&) = delete;
            CmndLineArgs& operator=(const CmndLineArgs&) = delete;
            CmndLineArgs& operator=(CmndLineArgs&&) = delete;

            /// \brief destructor (defualt)
            ~CmndLineArgs() {}

            /// \param flag Checks if flag is present in the command line arguments
            /// \return bool : true if the command line argument is present, false otherwise
            inline const bool ArgPresent(char flag) const { return (args.find(flag) != args.end()) ? true : false; }

            /// \param flag flag of the command line argument
            /// \return bool : true if the command line argument has a value, false otherwise
            inline const bool ArgHasValue(char flag) const { return (args.at(flag).has_value()) ? true : false; }

            /// \param flag flag of the command line argument
            /// \return std::string value of the command line argument
            /// \throw runtime error if the command line argument is not present
            inline const std::string GetArgValue(char flag) const { return args.at(flag).value(); }

    };

    /// \class Util
    /// \brief used to read and validate the source code from a file as well
    ///         as act as a store-all for miscallaneous utility functions
    class Util {
        public:

            static std::unique_ptr<CmndLineArgs> cmnd_line_args;

            // command line argument reading/storing and validation as well as version printing
            inline static const std::unique_ptr<CmndLineArgs>& GetCmndLineArgs() { return cmnd_line_args; }
            static const void PrintUsage();
            inline static const void PrintVersion() { std::cout << "< YLang > :: [0.0.2]" << std::endl; }
            static void ReadCmndLineArgs(int* argc , char* argv[]);

            /// \param token token to print
            static void PrintToken(const Token& token);
            
            /// \param file_path path to the file to read the source code from
            /// \return the source code from the file
            /// \throw runtime error if the file does not exist
            static std::string ReadSrc();

            /// \return name of the current file if the -f flag is present, "<no-file.yl>" otherwise
            static const std::string GetCurrFileName();
    };

}

#endif // !YLANG_UTIL_HPP