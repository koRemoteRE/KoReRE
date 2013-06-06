#pragma once
#include <Windows.h>
#include <iostream>
#include <string>

static class logger
{
public:
	logger(void);
	static void printTime(char *messege);
	static void clearConsole();
	~logger(void);
};

