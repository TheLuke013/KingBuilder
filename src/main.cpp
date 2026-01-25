#include "BuildSystem.h"

std::vector<std::string> GetArgs(int argc, char** argv) {
    std::vector<std::string> args;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            args.push_back(std::string(argv[i]));
        }
    }
    return args;
}

int main(int argc, char** argv) {
    BuildSystem buildSystem = BuildSystem();
    std::string buildDir = "./";
    bool cleanBuild = false;

    std::vector<std::string> args = GetArgs(argc, argv);
    if (!args.empty()) {
        for (size_t i = 0; i < args.size(); i++) {
            //std::cout << "Arg " << i << ": " << args[i] << std::endl;
            if (args[i] == "-relative") {
                buildDir = argv[1];
                std::cout << "Using relative build directory: " << buildDir << std::endl;
            } else if (args[i] == "-clean") {
                cleanBuild = true;
                std::cout << "Using clean build" << std::endl;
            }
        }
    }

    buildSystem.SetCleanBuild(cleanBuild);

    try {
        buildSystem.OpenBuildFile(buildDir);
        if (buildSystem.Compile()) {
            std::cout << "\nBuild was completed successful\n" << std::endl;
        } else {
            std::cout << "\nBuild was not completed successful. Error to build.\n" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }

	return 0;
}