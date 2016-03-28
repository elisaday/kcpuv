#include "header.h"
#include "log.h"

static void log(const char* txt) {
	time_t now;
	time(&now);
	struct tm* t = localtime(&now);
	printf("[%04d-%02d-%02d %02d:%02d:%02d] ", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	printf("%s\n", txt);
}

int log_info(const char* format, ...) {
	printf("[info] ");
	char buffer[1024];
	va_list arg;
	va_start(arg, format);
	vsprintf(buffer, format, arg);
	va_end(arg);
	log(buffer);
	return 0;
}

int log_err(const char* format, ...) {
	printf("[err]");
	char buffer[1024];
	va_list arg;
	va_start(arg, format);
	vsprintf(buffer, format, arg);
	va_end(arg);
	log(buffer);
	return 0;
}
