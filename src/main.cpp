#include <fstream>
#include <deque>

#include "fs.h"
#include "logger.h"

#include "tokenizer.h"
#include "parser.h"
#include "converter.h"

int main(int argc, char** argv) {
	if(argc == 1) {
		LogGlobalError(FatalError, "no input files");
	} else {
		std::vector<std::string> files;
		for(int i = 1; i < argc; ++i) {
			std::string file = argv[i];
			
			if(!fs::is_regular_file(file)) {
				LogGlobalError(Error, file + ": No such file or directory");
				return 1;
			}
			
			files.push_back(file);
		}

		for(auto & path : files) {
			auto res = tokenize(path);
			
			std::deque<Token> queue(res.begin(), res.end());
			queue.push_back(Token{ TokenType::Eof, "<EOF>" });
			
			Parser p(queue);
			auto ast = p.Parse();

			auto result = crpl(ast, path);

			if(!getError()) {
				std::string filename = path.substr(path.find_last_of("/\\") + 1);
				filename = filename.substr(0, filename.find_last_of('.'));
				
				std::ofstream out(filename + ".crpl");
				out << result;
				out.close();
			}
		}
	}
	
    return 0;
}