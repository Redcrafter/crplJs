#pragma once
#include <string>

class SourceLocation;

enum LogLevel {
	FatalError,
	Error,
	Warning,
};

bool getError();

void LogGlobalError(LogLevel level, const std::string& message);

void LogError(LogLevel level, const std::string& fileName, const std::string& message);
void LogError(LogLevel level, const SourceLocation& location, const std::string& message);
