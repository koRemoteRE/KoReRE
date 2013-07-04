#include "Server.h"

Server::Server(boost::asio::io_service &io_service, boost::asio::ip::tcp::endpoint &endpoint, unsigned short port)
	//: socketAcceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)){
	: socketAcceptor(io_service, endpoint){

		//socketAcceptor = boost::asio::ip::tcp::acceptor(io_service, endpoint);

		socketAcceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));

		connection_ptr new_conn(new Connection(socketAcceptor.get_io_service()));

		socketAcceptor.async_accept(new_conn->getSocket(),
			boost::bind(&Server::acceptHandler, this, 
			boost::asio::placeholders::error, new_conn));

		init();
}

Server::~Server(){}

void Server::init(){
	imageQueue = ImageQueue::getInstance();
	matrixQueue = MatrixQueue::getInstance();
}

void Server::acceptHandler(const boost::system::error_code &e, connection_ptr conn){
	if(!e){

		/*for (int i = 0; i < 1; i++){
			SerializableMatrix m = {0};
			m.id = i;

			matrixQueue->push(m);

		}*/

		conn->readAsync(mats, boost::bind(&Server::readHandler, this,
			boost::asio::placeholders::error));

		/*conn->writeAsync(,
			boost::bind(&Server::writeHandler, this,
			boost::asio::placeholders::error, conn));*/
	}
}

void Server::writeHandler(const boost::system::error_code &e, 
						  std::size_t bytes_transferred, 
						  connection_ptr conn){

	if(!e){

		/*conn->readAsync(,boost::bind(&Server::acceptHandler, this,
			boost::asio::placeholders::error, conn));*/
	}
}

void Server::readHandler(const boost::system::error_code &e){
	if(!e){
		for (std::size_t i = 0; i < mats.size(); ++i){
			matrixQueue->push(mats[i]);

			std::cout << "Got Matrix: " << mats[i].id << std::endl;
		}
	}
	connection_ptr newConn(new Connection(socketAcceptor.get_io_service()));
		
	socketAcceptor.async_accept(newConn->getSocket(),
		boost::bind(&Server::acceptHandler, this,
		boost::asio::placeholders::error, newConn));
}
