#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include "Log.h"

#define LOG_FILE "Debug\\gl.log"

void restart_log() {
	FILE *file;
	fopen_s(&file, LOG_FILE, "w");

	if (!file) {
		return;
	}

	time_t now = time(NULL);
	char date[26];
	ctime_s(date, 26, &now);

	fprintf(file, "LOG_FILE log. local time %s\n", date);

	fclose(file);
}

void log(const char* message, ...) {
	va_list argptr;

	FILE *file;
	fopen_s(&file, LOG_FILE, "a");

	if (!file) {
		return;
	}

	va_start(argptr, message);	// Variable argument number
	vfprintf(file, message, argptr);
	va_end(argptr);

	fclose(file);
}