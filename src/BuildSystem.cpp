#include "BuildSystem.h"

BuildSystem::BuildSystem() : buildDir(""), buildJson(JSON()), canBuild(false) {}

BuildSystem::~BuildSystem() {
    if (buildFile.is_open())
		buildFile.close();
}

bool BuildSystem::ValueIsGood(const std::string& value) {
	if (value.empty()) return false;

	return true;
}

bool BuildSystem::ValueIsArray(const std::string& value) {
	if (!buildJson.GetData().HasMember(value.c_str()) || !buildJson.GetData()[value.c_str()].IsArray()) {
		std::cout << "Build Error: '" << value << "' must be an array\n";
		return false;
	}

	return true;
}

bool BuildSystem::CheckBuildType(const std::string& type) {
	if (type != BuildTypes::DEBUG && type != BuildTypes::RELEASE && type != BuildTypes::DISTRIBUTION) {
		std::cout << "Build Error: '" << type << "' is not a valid build type\n";
		return false;
	}
	return true;
}

bool BuildSystem::CheckTargetType(const std::string& type) {
	if (type != TargetTypes::EXECUTABLE && type != TargetTypes::STATIC_LIB && type != TargetTypes::DYNAMIC_LIB) {
		std::cout << "Build Error: '" << type << "' is not a valid target type\n";
		return false;
	}
	return true;
}

bool BuildSystem::CheckArchitecture(const std::string& architecture) {
	if (architecture != Architectures::X86 && architecture != Architectures::X64) {
		std::cout << "Build Error: '" << architecture << "' is not a valid architecture\n";
		return false;
	}
	return true;
}

bool BuildSystem::CheckCXXStandard(const std::string& cxxStandard) {
	if (cxxStandard != CXXStandards::CXX98 &&
		cxxStandard != CXXStandards::CXX03 &&
		cxxStandard != CXXStandards::CXX11 &&
		cxxStandard != CXXStandards::CXX14 &&
		cxxStandard != CXXStandards::CXX17 &&
		cxxStandard != CXXStandards::CXX20 &&
		cxxStandard != CXXStandards::CXX23) {
		std::cout << "Build Error: '" << cxxStandard << "' is not a valid C++ standard\n";
		return false;
	}
	return true;
}

bool BuildSystem::CheckCompiler(const std::string& compiler) {
	if (compiler != Compilers::GXX &&
		compiler != Compilers::CLANG) {
		std::cout << "Build Error: '" << compiler << "' is not a valid compiler\n";
		return false;
	}
	return true;
}

std::vector<std::string> BuildSystem::GetFiles(const std::string& dirFiles) {
	try {
		std::vector<std::string> files;
	
	for (const auto& entry : std::filesystem::directory_iterator(dirFiles)) {
		if (entry.is_regular_file()) {
			files.push_back(entry.path().string());
		}
	}

	return files;
	} catch (const std::filesystem::filesystem_error& e) {
		std::cout << "Build Error: Filesystem Error: " << e.what() << std::endl;
		return {};
	}
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
		//PARSEIA O ARQUIVO DE BUILD
		buildJson.Parse(content);

		//OBTEM VALORES NO ARQUIVO DE BUILD E OS VERIFICA
		std::string type = buildJson.GetValue<std::string>(BuildKeys::TYPE, "");
		if (!ValueIsGood(type) || !CheckTargetType(type)) {
			std::cout << "Build Error: value ''" << BuildKeys::TYPE << "'' not is expected type" << std::endl;
			return;
		}
		buildStruct.type = type;

		std::string buildType = buildJson.GetValue<std::string>(BuildKeys::BUILD_TYPE, "");
		if (!ValueIsGood(buildType) || !CheckBuildType(buildType)) {
			std::cout << "Build Error: value ''" << BuildKeys::BUILD_TYPE << "'' not is expected type" << std::endl;
			return;
		}
		buildStruct.buildType = buildType;

		std::string architecture = buildJson.GetValue<std::string>(BuildKeys::ARCHITECTURE, "");
		if (!ValueIsGood(architecture) || !CheckArchitecture(architecture)) {
			std::cout << "Build Error: value ''" << BuildKeys::ARCHITECTURE << "'' not is expected type" << std::endl;
			return;
		}
		buildStruct.architecture = architecture;

		std::string outputName = buildJson.GetValue<std::string>(BuildKeys::OUTPUT_NAME, "");
		if (!ValueIsGood(outputName)) {
			std::cout << "Build Error: value ''" << BuildKeys::OUTPUT_NAME << "'' not is expected type" << std::endl;
			return;
		}
		buildStruct.outputName = outputName;

		std::string outputDir = buildJson.GetValue<std::string>(BuildKeys::OUTPUT_DIR, "");
		if (!ValueIsGood(outputDir)) {
			std::cout << "Build Error: value ''" << BuildKeys::OUTPUT_DIR << "'' not is expected type" << std::endl;
			return;
		}
		buildStruct.outputDir = outputDir;

		std::string cStandard = buildJson.GetValue<std::string>(BuildKeys::C_STANDARD, "");
		if (!ValueIsGood(cStandard) || !CheckCXXStandard(cStandard)) {
			std::cout << "Build Error: value ''" << BuildKeys::C_STANDARD << "'' not is expected type" << std::endl;
			return;
		}
		buildStruct.cStandard = cStandard;

		std::string compiler = buildJson.GetValue<std::string>(BuildKeys::COMPILER, "");
		if (!ValueIsGood(compiler) || !CheckCompiler(compiler)) {
			std::cout << "Build Error: value ''" << BuildKeys::COMPILER << "'' not is expected type" << std::endl;
			return;
		}
		buildStruct.compiler = compiler;

		//OBTEM ARRAY DE ARQUIVOS DE COMPILAÇÃO E O VERIFICA
		if (!ValueIsArray(BuildKeys::FILES_DIR)) {
			return;
		}

		rapidjson::GenericArray filesDir = buildJson.GetData()[BuildKeys::FILES_DIR].GetArray();
		if (filesDir.Empty()) {
			std::cout << "Build Error: not there no one file to compile in ''filesDir''." << std::endl;
			return;
		}

		//SALVA TODOS DIRETORIOS QUE DEVEM SER ITERADOS
		for (size_t i = 0; i < filesDir.Size(); i++) {
			rapidjson::GenericValue value = rapidjson::GenericValue(filesDir[i], buildJson.GetAllocator());
			std::string file = value.GetString();
			buildStruct.Dirfiles.push_back(buildDir + file);
		}

		//ITERA PELOS DIRETORIOS E ARMAZENA TODOS OS ARQUIVOS
		for (size_t i = 0; i < buildStruct.Dirfiles.size(); i++) {
			std::string dirFiles = buildStruct.Dirfiles[i];
			std::vector<std::string> dirFilesVector = GetFiles(dirFiles);
			buildStruct.files.insert(buildStruct.files.end(), dirFilesVector.begin(), dirFilesVector.end());
		}

		//VERIFICA SE O ARRAY DE INCLUDE DIRS POSSUI ALGUM ELEMENTO
		if (ValueIsArray(BuildKeys::INCLUDES_DIR)) {
			rapidjson::GenericArray includeDirs = buildJson.GetData()[BuildKeys::INCLUDES_DIR].GetArray();
			if (!includeDirs.Empty()) {
				for (size_t i = 0; i < includeDirs.Size(); i++) {
					rapidjson::GenericValue value = rapidjson::GenericValue(includeDirs[i], buildJson.GetAllocator());
					std::string includeDir = value.GetString();
					buildStruct.includeDirs.push_back(buildDir + includeDir);
				}
			}
		}

		//VERIFICA SE O ARRAY DE LIBS DIR POSSUI ALGUM ELEMENTO
		if (ValueIsArray(BuildKeys::LIBS_DIR)) {
			rapidjson::GenericArray libsDir = buildJson.GetData()[BuildKeys::LIBS_DIR].GetArray();
			if (!libsDir.Empty()) {
				for (size_t i = 0; i < libsDir.Size(); i++) {
					rapidjson::GenericValue value = rapidjson::GenericValue(libsDir[i], buildJson.GetAllocator());
					std::string libDir = value.GetString();
					buildStruct.libsDir.push_back(buildDir + libDir);
				}
			}
		}

		//VERIFICA SE O ARRAY DE LIBS POSSUI ALGUM ELEMENTO
		if (ValueIsArray(BuildKeys::LIBS)) {
			rapidjson::GenericArray libs = buildJson.GetData()[BuildKeys::LIBS].GetArray();
			if (!libs.Empty()) {
				for (size_t i = 0; i < libs.Size(); i++) {
					rapidjson::GenericValue value = rapidjson::GenericValue(libs[i], buildJson.GetAllocator());
					std::string lib = value.GetString();
					buildStruct.libs.push_back(lib);
				}
			}
		}

		//VERIFICA SE O ARRAY DE DEFINES POSSUI ALGUM ELEMENTO
		if (ValueIsArray(BuildKeys::DEFINES)) {
			rapidjson::GenericArray defines = buildJson.GetData()[BuildKeys::DEFINES].GetArray();
			if (!defines.Empty()) {
				for (size_t i = 0; i < defines.Size(); i++) {
					rapidjson::GenericValue value = rapidjson::GenericValue(defines[i], buildJson.GetAllocator());
					std::string define = value.GetString();
					buildStruct.defines.push_back(define);
				}
			}
		}

		//VERIFICA SE O ARRAY DE PRE BUILD COMMANDS POSSUI ALGUM ELEMENTO
		if (ValueIsArray(BuildKeys::PRE_BUILD_COMMANDS)) {
			rapidjson::GenericArray preBuildCommands = buildJson.GetData()[BuildKeys::PRE_BUILD_COMMANDS].GetArray();
			if (!preBuildCommands.Empty()) {
				for (size_t i = 0; i < preBuildCommands.Size(); i++) {
					rapidjson::GenericValue value = rapidjson::GenericValue(preBuildCommands[i], buildJson.GetAllocator());
					std::string command = value.GetString();
					buildStruct.preBuildCommands.push_back(command);
				}
			}
		}

		//VERIFICA SE O ARRAY DE POST BUILD COMMANDS POSSUI ALGUM ELEMENTO
		if (ValueIsArray(BuildKeys::POST_BUILD_COMMANDS)) {
			rapidjson::GenericArray postBuildCommands = buildJson.GetData()[BuildKeys::POST_BUILD_COMMANDS].GetArray();
			if (!postBuildCommands.Empty()) {
				for (size_t i = 0; i < postBuildCommands.Size(); i++) {
					rapidjson::GenericValue value = rapidjson::GenericValue(postBuildCommands[i], buildJson.GetAllocator());
					std::string command = value.GetString();
					buildStruct.postBuildCommands.push_back(command);
				}
			}
		}

		//IMPRIME AS INFORMAÇÕES DE BUILD
		std::cout << "\nCompilation Info:\n" << std::endl;
		for (size_t i = 0; i < buildStruct.files.size(); i++) {
			std::cout << "File to compile: " << buildStruct.files[i] << std::endl;
		}
		std::cout << buildStruct.files.size() << " files to compile found.\n" << std::endl;

		if (!buildStruct.includeDirs.empty()) {
			for (size_t i = 0; i < buildStruct.includeDirs.size(); i++) {
				std::cout << "Include Dir: " << buildStruct.includeDirs[i] << std::endl;
			}
		}

		if (!buildStruct.libsDir.empty()) {
			for (size_t i = 0; i < buildStruct.libsDir.size(); i++) {
				std::cout << "Libs Dir: " << buildStruct.libsDir[i] << std::endl;
			}
		}

		if (!buildStruct.libs.empty()) {
			for (size_t i = 0; i < buildStruct.libs.size(); i++) {
				std::cout << "Libs: " << buildStruct.libs[i] << std::endl;
			}
		}

		if (!buildStruct.defines.empty()) {
			for (size_t i = 0; i < buildStruct.defines.size(); i++) {
				std::cout << "Define: " << buildStruct.defines[i] << std::endl;
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

	//EXECUTA COMANDOS PRÉ BUILD
	if (!buildStruct.preBuildCommands.empty()) {
		std::cout << "\nExecuting Pre-Build Commands:\n" << std::endl;
		for (size_t i = 0; i < buildStruct.preBuildCommands.size(); i++) {
			std::string command = buildStruct.preBuildCommands[i];
			std::cout << "Pre-Build Command: " << command << std::endl;
			int commandResult = std::system(command.c_str());
			if (commandResult != 0) {
				std::cout << "\nFatal Error: Error to execute Pre-Build Command: " << command << std::endl;
				return false;
			}
		}
	}

	//VERIFICA VERSÃO DO COMPILADOR
	std::string compilerCheckCommand = buildStruct.compiler + " --version";
	int compilerCheckResult = std::system(compilerCheckCommand.c_str());
	if (compilerCheckResult != 0) {
		std::cout << "\nFatal Error: Compiler '" << buildStruct.compiler << "' not found." << std::endl;
		return false;
	}

	//BASE DOS COMANDOS DE COMPILAÇÃO
	std::string compileCommand = buildStruct.compiler;
	std::string libCompileCommand = "ar rcs ";

	//OUTPUTS
	std::string output = buildDir + buildStruct.outputDir;
	std::string objOutput = buildDir + buildStruct.outputDir;

	//ACRESCENTA TIPO DE BUILD AO DIRETÓRIO DOS OUTPUTS
	if (buildStruct.buildType == BuildTypes::DEBUG) {
		output += "debug/";
		objOutput += "debug/";
	} else if (buildStruct.buildType == BuildTypes::RELEASE) {
		output += "release/";
		objOutput += "release/";
	} else if (buildStruct.buildType == BuildTypes::DISTRIBUTION) {
		output += "distribution/";
		objOutput += "distribution/";
	}

	//ACRESCENTA ARQUITETURA AO DIRETÓRIO DE OBJETOS
	if (buildStruct.architecture == Architectures::X86) {
		output += "x86/";
		objOutput += "x86/";
		compileCommand += " -m32";
	} else if (buildStruct.architecture == Architectures::X64) {
		output += "x64/";
		objOutput += "x64/";
		compileCommand += " -m64";
	} else {
		std::cout << "Build Error: '" << buildStruct.architecture << "' is not a valid architecture\n";
		return false;
	}

	if (buildStruct.type == TargetTypes::DYNAMIC_LIB) {
		output += buildStruct.outputName + ".dll";
		objOutput += "obj/";
	} else if (buildStruct.type == TargetTypes::STATIC_LIB) {
		output += buildStruct.outputName + ".lib";
		objOutput += "obj/";
	} else {
		output += buildStruct.outputName + ".exe";
		objOutput += "obj/";
	}

	libCompileCommand += output;

	std::cout << "\nOutput Name: " << output << std::endl;
	std::cout << "Object Output Dir: " << objOutput << std::endl;

	//VERSÃO DO C++
	compileCommand += " -std=" + buildStruct.cStandard;

	//TIPO DE BUILD
	if (buildStruct.buildType == BuildTypes::DEBUG) {
		compileCommand += " -g -O0 -DDEBUG -Wall -Wextra";
	} else if (buildStruct.buildType == BuildTypes::RELEASE) {
		compileCommand += " -O2 -g -DNDEBUG -Wall -Wextra";
	} else if (buildStruct.buildType == BuildTypes::DISTRIBUTION) {
		compileCommand += " -O3 -DNDEBUG -s -Wall -Wextra";
	}

	if (buildStruct.buildType == BuildTypes::DISTRIBUTION &&
		buildStruct.type == TargetTypes::DYNAMIC_LIB) {
		compileCommand += " -fvisibility=hidden";
	}

	//ADICIONA INCLUDES AO COMANDO DE COMPILAÇÃO
	if (!buildStruct.includeDirs.empty()) {
		for (auto include : buildStruct.includeDirs) {
			compileCommand += " -I" + include;
		}
	}

	//ADICIONA DEFINES AO COMANDO DE COMPILAÇÃO
	if (!buildStruct.defines.empty()) {
		for (auto define : buildStruct.defines) {
			compileCommand += " -D" + define;
		}
	}


	//COMPILAÇÃO PARA OBJETOS

	//VERIFICA SE O DIRETÓRIO DE OBJETOS EXISTE
	if (!std::filesystem::exists(objOutput)) {
		std::filesystem::create_directories(objOutput);
	}

	//COMPILA CADA ARQUIVO PARA OBJETO
	std::vector<std::string> objectFiles;

	for (size_t i = 0; i < buildStruct.files.size(); i++) {
		std::string file = buildStruct.files[i];
		std::cout << "\nCompiling file: " << file << std::endl;

		std::filesystem::path filepath = std::filesystem::path(file);
		auto relative = std::filesystem::relative(filepath, buildDir);
		std::string safeName = relative.string();
		std::replace(safeName.begin(), safeName.end(), '/', '_');
		std::replace(safeName.begin(), safeName.end(), '\\', '_');

		std::string objFile = objOutput + safeName + ".o";

		if (std::filesystem::exists(objFile)) {
			if (std::filesystem::last_write_time(objFile) >
				std::filesystem::last_write_time(file)) {
				continue;
			}
		}

		std::string compileObjCommand = compileCommand;
		if (buildStruct.type == TargetTypes::DYNAMIC_LIB) {
			compileObjCommand += " -fPIC";
		}
		compileObjCommand += " -c " + file + " -o " + objFile;
		std::cout << "Compile Command: " << compileObjCommand << std::endl;

		//EXECUTA O COMANDO PARA COMPILAR O OBJETO
		int commandResult = std::system(compileObjCommand.c_str());
		if (commandResult != 0) {
			std::cout << "\nFatal Error: Error to compile file: " << file << std::endl;
			return false;
		}

		//ADICIONA O ARQUIVO OBJETO AO COMANDO FINAL DE LINKAGEM
		objectFiles.push_back(objFile);
	}

	if (objectFiles.empty()) {
		std::cout << "\nNothing to compile. All files are up to date." << std::endl;
		return true;
	}

	//ADICIONA ARQUIVOS OBJETOS AO COMANDO DE LINKAGEM
	for (auto objFile : objectFiles) {
		if (buildStruct.type == TargetTypes::DYNAMIC_LIB || buildStruct.type == TargetTypes::EXECUTABLE) {
			compileCommand += " " + objFile;
		} else if (buildStruct.type == TargetTypes::STATIC_LIB) {
			libCompileCommand += " " + objFile;
		}
	}

	//ADICIONA LIBS DIR AO COMANDO DE COMPILAÇÃO
	if (!buildStruct.libsDir.empty()) {
		for (auto libDir : buildStruct.libsDir) {
			compileCommand += " -L" + libDir;
		}
	}

	//ADICIONA LIBS AO COMANDO DE COMPILAÇÃO
	if (!buildStruct.libs.empty()) {
		for (auto lib : buildStruct.libs) {
			compileCommand += " -l" + lib;
		}
	}

	//RESETA O TARGET DE BUILD
	if (!std::filesystem::exists(output)) {
		std::filesystem::create_directories(output.substr(0, output.find_last_of('/')));
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
	compileCommand += " -o " + output;

	if (buildStruct.type == TargetTypes::STATIC_LIB) {
		std::cout << "\nLinking static library: " << output << std::endl;
		int commandResult = std::system(libCompileCommand.c_str());
		if (commandResult != 0) {
			std::cout << "\nFatal Error: Error to link static library" << std::endl;
			return false;
		}
		return true;
	}

	std::cout << "\nLinking output: " << output << std::endl;
	int commandResult = std::system(compileCommand.c_str());
	if (commandResult != 0) {
		std::cout << "\nFatal Error: Error to compile files" << std::endl;
		return false;
	}


	//EXECUTA COMANDOS PÓS BUILD
	if (!buildStruct.postBuildCommands.empty()) {
		std::cout << "\nExecuting post-build commands:\n" << std::endl;
		for (size_t i = 0; i < buildStruct.postBuildCommands.size(); i++) {
			std::string command = buildStruct.postBuildCommands[i];
			std::cout << "Executing command: " << command << std::endl;
			int postCommandResult = std::system(command.c_str());
			if (postCommandResult != 0) {
				std::cout << "\nWarning: Error to execute post-build command: " << command << std::endl;
			}
		}
	}

	return true;
}