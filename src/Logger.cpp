#include "Logger.h"
#include <stdio.h>
#include <cassert>
#include <stdarg.h>
#include <time.h>

static FILE *logFile = nullptr;

void InitLogger()
{
	assert(logFile == nullptr);
	fopen_s(&logFile, "device16.log", "a");
	assert(logFile);
}

void Log(const char* fmt, ...)
{
	if (logFile == nullptr)
	{
		return;
	}

	char msg[4096];
	va_list args;
	va_start(args, fmt);
	vsnprintf(msg, sizeof(msg), fmt, args);
	va_end(args);

	struct tm tm;
	time_t now;
	time(&now);
	localtime_s(&tm, &now);

	fprintf(logFile, "%04d-%02d-%02d %02d:%02d:%02d: %s\n",
		tm.tm_year + 1900,
		tm.tm_mon + 1,
		tm.tm_mday,
		tm.tm_hour,
		tm.tm_min,
		tm.tm_sec,
		msg
	);
}


