#ifndef BUILD_H
#define BUILD_H

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>

struct BuildStruct {
    std::string outputName;
    std::string outputDir;
    std::string objOutput;
    std::string cppVersion;
    std::vector<std::string> files; 
    std::vector<std::string> includeDirs; 
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
        bool canBuild;

        bool ValueIsGood(const std::string& value);
};

#endif