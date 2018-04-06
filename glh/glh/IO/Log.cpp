#include "Log.h"

const std::string Log::logLevelPrefixes[] =
{
	"TRACE",
	"DEBUG",
	"INFO",
	"WARNING",
	"ERROR"
};

int Log::logLevel = Log::LOG_TRACE;

void Log::SetLogLevel(int level)
{
	Log::logLevel = level;
}

void Log::Write(int level, std::string message)
{
	if (level < Log::LOG_TRACE || level >= Log::LOG_NONE)
		return;

	if (level < Log::logLevel)
		return;

	std::string formattedMessage = "[" + std::to_string(Util::getTime()) + "] " + logLevelPrefixes[level] + ": " + message;
	std::cout << formattedMessage << std::endl;
}

void Log::WriteTrace(std::string message) {
	Log::Write(Log::LOG_TRACE, message);
}
void Log::WriteDebug(std::string message) {
	Log::Write(Log::LOG_DEBUG, message);
}
void Log::WriteInfo(std::string message) {
	Log::Write(Log::LOG_INFO, message);
}
void Log::WriteWarning(std::string message) {
	Log::Write(Log::LOG_WARNING, message);
}
void Log::WriteError(std::string message) {
	Log::Write(Log::LOG_ERROR, message);
}