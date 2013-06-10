#pragma once
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

