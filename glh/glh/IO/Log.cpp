#include "Log.h"

const std::string Log::logLevelPrefixes[] =
{
	"TRACE",
	"DEBUG",
	"INFO",
	"WARNING",
	"ERROR"
};

void Log::Write(int level, std::string message)
{
	if (level < LOG_TRACE || level >= LOG_NONE)
		return;

	std::string formattedMessage = "[" + std::to_string(Util::getTime()) + "] " + logLevelPrefixes[level] + ": " + message;
	std::cout << formattedMessage << std::endl;
}
