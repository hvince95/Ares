#pragma once

#include <iostream>
#include <string>

#include "..\Core\Util.h"

class Log
{
public:

	/// Trace message level.
	static const int LOG_TRACE = 0;
	/// Debug message level.
	static const int LOG_DEBUG = 1;
	/// Informative message level.
	static const int LOG_INFO = 2;
	/// Warning message level.
	static const int LOG_WARNING = 3;
	/// Error message level.
	static const int LOG_ERROR = 4;
	/// Disable all log messages.
	static const int LOG_NONE = 5;

	static void Write(int level, std::string message);
private:
	static int logLevel;

	static const std::string logLevelPrefixes[];
};