#include "BuildSystem.h"

int main(int argc, char** argv) {
    BuildSystem buildSystem = BuildSystem();
    std::string buildDir = "./";

    if (argc > 1) {
        if (argc >= 2 && argv[2] == "-relative") {
            buildDir += argv[1];
        } else {
            buildDir = argv[1];
        }
        std::cout << "Starting building in " << argv[1] << std::endl;
    }

    buildSystem.OpenBuildFile(buildDir);
    if (buildSystem.Compile()) {
        std::cout << "\nBuild was completed successful\n" << std::endl;
    } else {
        std::cout << "\nBuild was not completed successful. Error to build.\n" << std::endl;
    }

    system("pause");

	return 0;
}