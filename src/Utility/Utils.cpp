#include "Utils.h"
#include <fstream>
#include <memory>
#include <map>
#include <sstream>

/*
 * Utils.cpp
 *
 *  Created on: 29.03.2016
 *      Author: tly
 */

std::string fileToString(const std::string& fileName){
	std::ifstream file(fileName,std::ios::binary);
	if(file){
		auto start = file.tellg();
		file.seekg(0,std::ios::end);
		auto fileSize = file.tellg() - start;
		file.seekg(0,std::ios::beg);

		auto array = std::make_unique<char[]>(fileSize);
		file.read(array.get(),fileSize);
		return std::string(array.get(),array.get()+fileSize);
	} else {
		throw std::runtime_error("file "+ fileName + " could not be opened.");
	}
}

std::string demangle(const std::type_info& info){
	return demangle(info.name());
}

std::string loadSourceFileWithHeaders(const std::string& fileName) {
	std::set<std::string> alreadyLoadedFiles;
	return loadSourceFileWithHeaders(fileName,alreadyLoadedFiles);
}

std::string loadSourceFileWithHeaders(
	const std::string& fileName,
	std::set<std::string>& alreadyLoadedFiles
) {
	std::string sourceFile = fileToString(fileName);
	alreadyLoadedFiles.insert(fileName);

	size_t lastSeparatorPos = fileName.find_last_of('/');
	std::string path(lastSeparatorPos == std::string::npos ?
		"." :
		fileName.substr(0,lastSeparatorPos)
	);
	//search for includes
	size_t pos = 0;
	while((pos = sourceFile.find("#include \"",pos)) != std::string::npos){
		size_t fileNameStart = pos + sizeof("include \"");
		size_t quotePos = sourceFile.find('\"',fileNameStart);
		if(quotePos == std::string::npos){
			throw std::runtime_error("include in file " + fileName + " is not properly formatted.");
		}
		std::string headerFileName = sourceFile.substr(fileNameStart,quotePos - fileNameStart);
		std::string headerFilePath = path + '/' + headerFileName; //TODO normalize path

		size_t lineNumber = std::count(sourceFile.begin(),sourceFile.begin()+quotePos,'\n');

		std::string headerCode = "";
		if(!alreadyLoadedFiles.count(headerFilePath)){
			headerCode =
				"#line 1 \"" + headerFileName + "\"\n" +
				loadSourceFileWithHeaders(headerFilePath) +
				"#line " + std::to_string(lineNumber) + " \"" + fileName + "\"\n";
		}

		sourceFile.replace(
			sourceFile.begin() + pos,sourceFile.begin() + quotePos + 1,
			headerCode
		);
		pos += headerCode.size() - (quotePos - pos);
	}
	return sourceFile;
}

#if defined(__GNUC__) or defined(__clang__)
#include <cxxabi.h>

std::string demangle(const char* mangledName){
	size_t length = 256;
	int error;
	std::unique_ptr<char,decltype(&std::free)> demangledName(
		abi::__cxa_demangle(mangledName,nullptr,&length,&error),
		&std::free
	);
	if(error){
		return mangledName;
	}else{
		return std::string((const char*) demangledName.get());
	}
}
#else
std::string demangle(const char* mangledName){
    return mangledName;
}
#endif

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

//http://stackoverflow.com/questions/478898/how-to-execute-a-command-and-get-output-of-command-within-c-using-posix
std::pair<int,std::string> systemCommand(const std::string& command) {
	std::string redirectedCommand = command + " 2>&1";
	auto pipe = popen(redirectedCommand.c_str(),"r");

	if(!pipe){
		throw std::runtime_error("could not open pipe.");
	}

	try{
	    char buffer[128];
	    std::string result = "";
	    while (!feof(pipe)) {
	        if (fgets(buffer, 128, pipe) != NULL)
	            result += buffer;
	    }
	    return {
	    	pclose(pipe), //pclose returns the exit code of the called command
	    	result
	    };
	}catch(...){
		//something went wrong with the string, close the pipe.
		pclose(pipe);
		std::rethrow_exception(std::current_exception());
	}
}

#if defined(__linux) || defined(__unix)
	#include <unistd.h>
	std::string getCurrentWorkingDirectory() {
		char currentPath[FILENAME_MAX];
		getcwd(currentPath,sizeof(currentPath));
		return std::string((const char*)currentPath);
	}
#elif defined(__APPLE__)
	#error "Utils for apple not implemented"
#elif defined(_WIN32)
	#include <direct.h>
	std::string getCurrentWorkingDirectory() {
		char currentPath[FILENAME_MAX];
		_getcwd(currentPath,sizeof(currentPath));
		return std::string((const char*)currentPath);
}

#else
	#error "os not detected."
#endif


bool fileExists(const std::string& fileName) {
	std::ifstream file(fileName);
	return bool(file);
}

std::string quote(const std::string& s){
	std::stringstream ss;
	static std::map<char,std::string> escapeMap{
		{'\\',"\\\\"},
		{'"',"\\\""},
		{'/',"\\/"},
		{'\b',"\\b"},
		{'\f',"\\f"},
		{'\r',"\\r"},
		{'\n',"\\n"},
		{'\t',"\\t"},
	};

	ss << "\"";
	for(char c : s){
		if(escapeMap.count(c)){
			ss << escapeMap.at(c);
		}else{
			ss << c;
		}
	}
	ss << "\"";
	return ss.str();
}

std::string unquote(const std::string& s){
	std::stringstream ss;
	assertOrThrow(s.size() >= 2);
	assertOrThrow(s.front() == '"' && s.back() == '"');
	static std::map<char,char> unescapeMap{
		{'\\','\\'},
		{'"','"'},
		{'/','/'},
		{'b','\b'},
		{'f','\f'},
		{'r','\r'},
		{'n','\n'},
		{'t','\t'},
	};

	for(uint32_t i = 1; i < s.size()-1; i++){
		if(s.at(i) == '\\'){
			i++;
			ss << unescapeMap.at(s.at(i));
		}else{
			ss << s.at(i);
		}
	}
	return ss.str();
}
