// renderlight.h
//

#pragma once

#include <windows.h>
#include <stdio.h>

#undef MAX_VALUE

int LoadFile(const char *fileName, void **buffer);

/*
============
LogPrint
============
*/
__forceinline void LogPrint(const char *format, ...) {
	va_list argptr;
#define MAX_VA_STRING   32000
	static char temp_buffer[MAX_VA_STRING];
	static int index = 0;
	char    *buf;
	int len;


	va_start(argptr, format);
	vsprintf(temp_buffer, format, argptr);
	va_end(argptr);

	printf(temp_buffer);
	OutputDebugStringA(temp_buffer);
}

/*
============
LogPrint
============
*/
__forceinline void LogError(const char *format, ...) {
	va_list argptr;
#define MAX_VA_STRING   32000
	static char temp_buffer[MAX_VA_STRING];
	static int index = 0;
	char    *buf;
	int len;


	va_start(argptr, format);
	vsprintf(temp_buffer, format, argptr);
	va_end(argptr);

	printf(temp_buffer);
	OutputDebugStringA(temp_buffer);

	exit(1);
}
