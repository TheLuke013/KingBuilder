#ifndef BUILD_H
#define BUILD_H

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>

#include "JSON.h"

namespace BuildKeys {
    constexpr const char* OUTPUT_NAME   = "outputName";
    constexpr const char* OUTPUT_DIR    = "outputDir";
    constexpr const char* OBJ_OUTPUT    = "objOutput";
    constexpr const char* CPP_VERSION   = "cppVersion";
    constexpr const char* FILES_DIR     = "filesDir";
    constexpr const char* INCLUDES_DIR  = "includesDir";
    constexpr const char* LIBS_DIR      = "libsDir";
    constexpr const char* LIBS          = "libs";
    constexpr const char* DEFINES       = "defines";
};

struct BuildStruct {
    std::string outputName;
    std::string outputDir;
    std::string objOutput;
    std::string cppVersion;
    std::vector<std::string> Dirfiles;
    std::vector<std::string> files;
    std::vector<std::string> includeDirs;
    std::vector<std::string> libsDir;
    std::vector<std::string> libs;
    std::vector<std::string> defines;
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
        std::vector<std::string> GetFiles(const std::string& dirFiles);
};

#endif