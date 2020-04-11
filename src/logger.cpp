#include "logger.h"
#include "colors.h"
#include "tokenizer.h"

#include <iostream>

bool error = false;

bool getError() {
	return error;
}

static void LogError(LogLevel level, const std::string& message) {
	switch(level) {
		case FatalError:
			error = true;
			std::cout << FgBrightRed << "fatal error: " << Reset;
			break;
		case Error:
			error = true;
			std::cout << FgBrightRed << "error: " << Reset;
			break;
		case Warning:
			std::cout << FgBrightMagenta << "error: " << Reset;
			break;
	}
	std::cout << message << std::endl;
}

void LogGlobalError(LogLevel level, const std::string& message) {
	std::cout << "crpl: ";
	LogError(level, message);
}

void LogError(LogLevel level, const std::string& fileName, const std::string& message) {
	std::cout << fileName << ": ";
	LogError(level, message);
}

void LogError(LogLevel level, const SourceLocation& location, const std::string& message) {
	std::cout << location.fileName << ':' << location.line << ':' << location.column << ": ";
	LogError(level, message);
}
