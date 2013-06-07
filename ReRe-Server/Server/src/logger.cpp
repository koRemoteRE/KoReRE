#include "logger.h"

bool logging = true;

logger::logger(void)
{

}


logger::~logger(void)
{
}

void logger::printTime(char *messege){
	if(logging){
		std::cout << GetTickCount() << " - - - " << messege << std::endl;
	}
}

void logger::clearConsole(){
	if(logging){
		system("cls");
	}
}
