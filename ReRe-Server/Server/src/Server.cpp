#include "Server.h"

Server::Server(void){
	imageQueue = ImageQueue::getInstance();
	matrixQueue = MatrixQueue::getInstance();
}

Server::~Server(void){}

void Server::start(){

}

void Server::acceptHandler(const boost::system::error_code &ec){

}

void Server::writeHandler(const boost::system::error_code &ec, std::size_t bytes_transferred){

}
