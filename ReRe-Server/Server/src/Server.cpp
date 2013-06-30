#include "Server.h"

Server::Server(boost::asio::io_service &io_service, unsigned short port)
	: socketAcceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)){
	
		connection_ptr new_conn(new Connection(socketAcceptor.get_io_service()));

		socketAcceptor.async_accept(new_conn->getSocket(),
			boost::bind(&Server::acceptHandler, this, 
			boost::asio::placeholders::error, new_conn));
}

Server::~Server(){}

void Server::init(){
	imageQueue = ImageQueue::getInstance();
	matrixQueue = MatrixQueue::getInstance();
}

void Server::start(){

}

void Server::acceptHandler(const boost::system::error_code &e, connection_ptr conn){
	if(!e){
		/*conn->writeAsync(,
			boost::bind(&Server::writeHandler, this,
			boost::asio::placeholders::error, conn));*/
	}

	connection_ptr newConn(new Connection(socketAcceptor.get_io_service()));
	socketAcceptor.async_accept(newConn->getSocket(),
		boost::bind(&Server::acceptHandler, this,
		boost::asio::placeholders::error, newConn));
}

void Server::writeHandler(const boost::system::error_code &ec, std::size_t bytes_transferred){

}
