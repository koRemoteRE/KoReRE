#include "logger.h"

bool logging = true;

logger::logger(void)
{

}


logger::~logger(void)
{
}

void logger::printTime(const char *messege){
	if(logging){
		std::cout << GetTickCount() << " - - - " << messege << std::endl;
	}
}

void logger::printInt(int n){
	if(logging){
		std::cout << n << std::endl;
	}
}

void logger::clearConsole(){
	if(logging){
		system("cls");
	}
}
