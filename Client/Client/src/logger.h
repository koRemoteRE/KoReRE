#ifdef _WIN32 
# define _WIN32_WINNT 0x0501 
#endif 

#ifndef LOGGER
#define LOGGER
#include <Windows.h>
#include <iostream>
#include <string>

class logger
{
public:
	logger(void);
	static void printTime(const char *messege);
	static void printInt(int n);
	static void clearConsole();
	~logger(void);
};

#endif

