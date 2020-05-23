#include <fstream>
#include <deque>
#include <iostream>

#include "fs.h"
#include "logger.h"
#include "colors.h"
#include "fileWatcher.h"

#include "tokenizer.h"
#include "parser.h"
#include "converter.h"

const int majorVersion = 1;
const int minorVersion = 1;
const int patchVersion = 0;

static bool hasOut = false;
static std::string outputPath;

static bool CompileFile(std::string path) {
	std::vector<Token> res = tokenize(path);

	try {
		res = tokenize(path);
	} catch(std::exception& e) {
		Logger::Log(Error, path, e.what());
	}

	// tokenize error
	if(Logger::getError()) {
		Logger::clearError();
		return false;
	}

	std::deque<Token> queue(res.begin(), res.end());
	queue.push_back(Token{ Tokentype::Eof, "<EOF>" });

	Parser p(queue);
	auto ast = p.Parse();

	// code error
	if(Logger::getError()) {
		Logger::clearError();
		return false;
	}

	auto result = crpl(ast, path);

	// convert error
	if(Logger::getError()) {
		Logger::clearError();
		return false;
	}
	std::string filename = path.substr(path.find_last_of("/\\") + 1);
	filename = filename.substr(0, filename.find_last_of('.'));

	std::string outFile;
	if(hasOut) {
		outFile = outputPath + "/" + filename;
	} else {
		outFile = path.substr(0, path.find_last_of("/\\")) + filename;
	}
	outFile += ".crpl";

	std::ofstream out(outFile);
	out << result;
	out.close();

	for(auto& i : ast) {
		delete i;
	}

	return true;
}

int main(int argc, char** argv) {
#ifdef _WIN32
	LoadDefaultColor();
#endif

	std::vector<std::string> inputParams;
	std::vector<std::string> inputFiles;

	bool watch = false;

	for(int i = 1; i < argc; i++) {
		std::string line = argv[i];

		if(line[0] == '-') {
			// is argument
			if(line == "-h" || line == "--help") {
				std::cout << "Version " << majorVersion << '.' << minorVersion << '.' << patchVersion << '\n';
				std::cout << "Usage crpl [options] file...\n\n";
				std::cout << "Options:\n";
				std::cout << " -h, --help   Print this message.\n";
				std::cout << " -o, --out    Redirect output files to directory.\n";
				std::cout << " -w, --watch  Watch input files.\n";
				return 0;
			} else if(line == "-o" || line == "--out") {
				i++;
				std::string next = argv[i];

				if(fs::is_directory(next)) {
					hasOut = true;
					outputPath = next;
				} else {
					Logger::LogGlobal(Error, "Output path has to be directory");
				}
			} else if(line == "-w" || line == "--watch") {
				watch = true;
			} else {
				Logger::LogGlobal(Error, "unrecognized command line option '" + line + "'");
				return 1;
			}
		} else {
			if(fs::is_directory(line)) {
				inputParams.push_back(line);
				for(auto& f : fs::recursive_directory_iterator(line)) {
					inputFiles.push_back(f.path().string());
				}
			} else if(fs::is_regular_file(line)) {
				inputParams.push_back(line);
				inputFiles.push_back(line);
			} else {
				Logger::LogGlobal(Error, line + ": No such file or directory");
			}
		}
	}

	if(inputFiles.empty()) {
		Logger::LogGlobal(FatalError, "no input files");
	} else {
		for(auto& path : inputFiles) {
			CompileFile(path);
		}
	}

	if(watch) {
		std::cout << "Started watching files" << std::endl;

		FileWatcher watcher(inputParams);

		watcher.start([](std::string file, FileStatus status) -> void {
			if(!fs::is_regular_file(file)) {
				return;
			}

			fs::path path = fs::canonical(file);
			if(path.extension() == ".js") {
				if(CompileFile(file)) {
					std::cout << "Compiled " << path << std::endl;
				}
			}
		});
	}
	
	return 0;
}
