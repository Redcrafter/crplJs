#pragma once
#include <string>

class SourceLocation;

enum LogLevel {
	FatalError,
	Error,
	Warning,
};

namespace Logger { 
	void LogGlobal(LogLevel level, const std::string& message);

	void Log(LogLevel level, const std::string& fileName, const std::string& message);
	void Log(LogLevel level, const SourceLocation& location, const std::string& message);

	bool getError();
	bool clearError();
}
