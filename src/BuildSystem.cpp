#include "BuildSystem.h"
#include "JSON.h"

BuildSystem::BuildSystem() : canBuild(false), buildDir("") {}

BuildSystem::~BuildSystem() {
    if (buildFile.is_open())
		buildFile.close();
}

bool BuildSystem::ValueIsGood(const std::string& value) {
	if (value.empty()) return false;

	return true;
}

void BuildSystem::OpenBuildFile(const std::string& buildDir) {
	//MONTA DIRETORIO PARA O ARQUIVO DE BUILD
	this->buildDir = buildDir;
	std::filesystem::path filejson = "build.json";
	std::filesystem::path buildpath = buildDir;
	std::filesystem::path filename = std::filesystem::current_path() / buildpath / filejson;

	buildFile.open(filename, std::ios::in);

	if (buildFile.is_open() && buildFile.good()) {
		std::string content((std::istreambuf_iterator<char>(buildFile)), std::istreambuf_iterator<char>());
		
		//VERIFICA SE O ARQUIVO NÃO ESTÁ VAZIO
		if (content.empty()) {
			std::cout << "Build Error: Empty build file." << std::endl;
			buildFile.close();
			return;
		}

		JSON buildJson = JSON();
		buildJson.Parse(content);

		//OBTEM VALORES NO ARQUIVO DE BUILD E OS VERIFICA
		std::string outputName = buildJson.GetValue<std::string>("outputName", "");
		if (!ValueIsGood(outputName)) {
			std::cout << "Build Error: value ''outputName'' not is expected type" << std::endl;
			return;
		}
		buildStruct.outputName = outputName;

		std::string outputDir = buildJson.GetValue<std::string>("outputDir", "");
		if (!ValueIsGood(outputDir)) {
			std::cout << "Build Error: value ''outputDir'' not is expected type" << std::endl;
			return;
		}
		buildStruct.outputDir = outputDir;

		std::string objOutput = buildJson.GetValue<std::string>("objOutput", "");
		if (!ValueIsGood(objOutput)) {
			std::cout << "Build Error: value ''objOutput'' not is expected type" << std::endl;
			return;
		}
		buildStruct.objOutput = objOutput;

		std::string cppVersion = buildJson.GetValue<std::string>("cppVersion", "");
		if (!ValueIsGood(cppVersion)) {
			std::cout << "Build Error: value ''cppVersion'' not is expected type" << std::endl;
			return;
		}
		buildStruct.cppVersion = cppVersion;

		//OBTEM ARRAY DE ARQUIVOS DE COMPILAÇÃO E O VERIFICA
		rapidjson::GenericArray files = buildJson.GetData()["files"].GetArray();
		if (files.Empty()) {
			std::cout << "Build Error: not there no one file to compile in ''files''." << std::endl;
			return;
		}

		for (int i = 0; i < files.Size(); i++) {
			rapidjson::GenericValue value = rapidjson::GenericValue(files[i], buildJson.GetAllocator());
			std::string file = value.GetString();
			buildStruct.files.push_back(buildDir + file);
		}

		//VERIFICA SE O ARRAY DE INCLUDE DIRS POSSUI ALGUM ELEMENTO
		rapidjson::GenericArray includeDirs = buildJson.GetData()["includeDirs"].GetArray();
		if (!includeDirs.Empty()) {
			for (int i = 0; i < includeDirs.Size(); i++) {
				rapidjson::GenericValue value = rapidjson::GenericValue(includeDirs[i], buildJson.GetAllocator());
				std::string includeDir = value.GetString();
				buildStruct.includeDirs.push_back(buildDir + includeDir);
			}
		}

		canBuild = true;
		buildFile.close();
	} else {
		std::cout << "Build Error: Not found " << filejson << " file." << std::endl;
	}
}

bool BuildSystem::Compile() {
	if (!canBuild) {
		return false;
	}

	std::string output = buildStruct.outputDir + buildStruct.outputName;
	std::string compileCommand = "g++ -o " + output;

	for (auto file : buildStruct.files) {
		compileCommand += " " + file;
	}

	if (!buildStruct.includeDirs.empty()) {
		for (auto include : buildStruct.includeDirs) {
			compileCommand += " -I" + include;
		}
	}

	compileCommand += " --std=" + buildStruct.cppVersion;

	//RESETA O TARGET DE BUILD
	if (!std::filesystem::exists(buildStruct.outputDir)) {
		std::filesystem::create_directories(buildStruct.outputDir);
	} else {
		std::fstream outputFile;
	    outputFile.open(output, std::ios::in);

		if (outputFile.is_open()) {
			outputFile.close();
			if (!std::filesystem::remove(output)) {
				std::cout << "Fatal Error: Not was possible remove output file" << std::endl;
				return false;
			}
		}
	}

	//EXECUTA O COMANDO PARA COMPILAR
	std::system("g++ --version");

	int commandResult = std::system(compileCommand.c_str());
	if (commandResult != 0) {
		std::cout << "\nFatal Error: Error to compile files" << std::endl;
		return false;
	}

	return true;
}