#ifndef BUILD_H
#define BUILD_H

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>

#include "JSON.h"

namespace BuildKeys {
    constexpr const char* TYPE                = "type";
    constexpr const char* BUILD_TYPE          = "buildType";
    constexpr const char* ARCHITECTURE        = "architecture";
    constexpr const char* OUTPUT_NAME         = "outputName";
    constexpr const char* OUTPUT_DIR          = "outputDir";
    constexpr const char* C_STANDARD          = "cStandard";
    constexpr const char* COMPILER            = "compiler";
    constexpr const char* FILES_DIR           = "filesDir";
    constexpr const char* INCLUDES_DIR        = "includesDir";
    constexpr const char* LIBS_DIR            = "libsDir";
    constexpr const char* LIBS                = "libs";
    constexpr const char* DEFINES             = "defines";
    constexpr const char* PRE_BUILD_COMMANDS  = "preBuildCommands";
    constexpr const char* POST_BUILD_COMMANDS = "postBuildCommands";
};

namespace TargetTypes {
    constexpr const char* EXECUTABLE    = "executable";
    constexpr const char* STATIC_LIB    = "staticLib";
    constexpr const char* DYNAMIC_LIB   = "dynamicLib";
};

namespace BuildTypes {
    constexpr const char* DEBUG         = "debug";
    constexpr const char* RELEASE       = "release";
    constexpr const char* DISTRIBUTION  = "distribution";
};

namespace Architectures {
    constexpr const char* X86          = "x86";
    constexpr const char* X64          = "x64";
};

namespace CXXStandards {
    constexpr const char* CXX98 = "c++98";
    constexpr const char* CXX03 = "c++03";
    constexpr const char* CXX11 = "c++11";
    constexpr const char* CXX14 = "c++14";
    constexpr const char* CXX17 = "c++17";
    constexpr const char* CXX20 = "c++20";
    constexpr const char* CXX23 = "c++23";
}

namespace Compilers {
    constexpr const char* GXX          = "g++";
    constexpr const char* CLANG        = "clang++";
};

struct BuildStruct {
    std::string type;
    std::string buildType;
    std::string architecture;
    std::string outputName;
    std::string outputDir;
    std::string cStandard;
    std::string compiler;
    std::vector<std::string> Dirfiles;
    std::vector<std::string> files;
    std::vector<std::string> includeDirs;
    std::vector<std::string> libsDir;
    std::vector<std::string> libs;
    std::vector<std::string> defines;
    std::vector<std::string> preBuildCommands;
    std::vector<std::string> postBuildCommands;
};

class BuildSystem {
    public:
        BuildSystem();
        ~BuildSystem();

        void OpenBuildFile(const std::string& buildDir);
        bool Compile();

    private:
        std::fstream buildFile;
        std::string buildDir;
        BuildStruct buildStruct;
        JSON buildJson;
        bool canBuild;

        bool ValueIsGood(const std::string& value);
        bool ValueIsArray(const std::string& value);
        bool CheckBuildType(const std::string& type);
        bool CheckTargetType(const std::string& type);
        bool CheckArchitecture(const std::string& architecture);
        bool CheckCXXStandard(const std::string& cxxStandard);
        bool CheckCompiler(const std::string& compiler);

        std::vector<std::string> GetFiles(const std::string& dirFiles);
};

#endif