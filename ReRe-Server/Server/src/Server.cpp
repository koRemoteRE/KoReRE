#include "Server.h"
#include "logger.h"

Server::Server(boost::asio::io_service &io_service, boost::asio::ip::tcp::endpoint &endpoint, unsigned short port)
	: socketAcceptor(io_service, endpoint){

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
		conn->readAsync(mats, 
			boost::bind(&Server::readHandler, this,
			boost::asio::placeholders::error,
			std::size_t(1024), conn));

	}
}

void Server::writeHandler(const boost::system::error_code &e, 
						  std::size_t bytes_transferred, 
						  connection_ptr conn){
	if(!e){
		logger::printTime("Write Data end.");
		images.clear();
	}else{
		std::cout << e.message() << std::endl;
	}

	connection_ptr new_conn(new Connection(socketAcceptor.get_io_service()));

		socketAcceptor.async_accept(new_conn->getSocket(),
			boost::bind(&Server::acceptHandler, this, 
			boost::asio::placeholders::error, new_conn));
}

void Server::readHandler(const boost::system::error_code &e, 
						 std::size_t bytes_transferred,
						 connection_ptr conn){
	if(!e){
		for (std::size_t i = 0; i < mats.size(); ++i){
			matrixQueue->push(mats[i]);

			std::cout << "Got Matrix: " << mats[i].id << std::endl;
		}

		mats.clear();

		conn->readAsync(mats, 
			boost::bind(&Server::readHandler, this,
			boost::asio::placeholders::error,
			std::size_t(1024), conn));

		SerializableImage img;
		imageQueue->waitAndPop(img);
		images.push_back(img);

		logger::printTime("Sending...");
		conn->writeAsync(images, 
			boost::bind(&Server::writeHandler, this,
			boost::asio::placeholders::error,
			std::size_t(1024), conn));

	}else{

      std::cerr << e.message() << std::endl;
	  connection_ptr new_conn(new Connection(socketAcceptor.get_io_service()));

		socketAcceptor.async_accept(new_conn->getSocket(),
			boost::bind(&Server::acceptHandler, this, 
			boost::asio::placeholders::error, new_conn));
	}
}
