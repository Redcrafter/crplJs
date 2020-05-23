#include <fstream>
#include <deque>

#include "fs.h"
#include "logger.h"
#include "colors.h"

#include "tokenizer.h"
#include "parser.h"
#include "converter.h"

int main(int argc, char** argv) {
	#ifdef _WIN32
	LoadDefaultColor();
	#endif

	if(argc == 1) {
		Logger::LogGlobal(FatalError, "no input files");
		return 1;
	} else {
		std::vector<std::string> files;
		for(int i = 1; i < argc; ++i) {
			std::string file = argv[i];
			
			if(!fs::is_regular_file(file)) {
				Logger::LogGlobal(Error, file + ": No such file or directory");
				return 1;
			}
			
			files.push_back(file);
		}

		for(auto & path : files) {
			auto res = tokenize(path);
			
			std::deque<Token> queue(res.begin(), res.end());
			queue.push_back(Token{ Tokentype::Eof, "<EOF>" });
			
			Parser p(queue);
			auto ast = p.Parse();

			auto result = crpl(ast, path);

			if(!Logger::getError()) {
				std::string filename = path.substr(path.find_last_of("/\\") + 1);
				filename = filename.substr(0, filename.find_last_of('.'));
				
				std::ofstream out(filename + ".crpl");
				out << result;
				out.close();
			}

			for(auto& i : ast) {
				delete i;
			}
		}
	}
	
    return 0;
}
