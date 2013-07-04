#include "Client.h"
#include "logger.h"


Client::Client(boost::asio::io_service& io_service,
      const std::string& host, const std::string& service)
	  :conn(io_service)
{
	boost::asio::ip::tcp::resolver resolver(io_service);
	boost::asio::ip::tcp::resolver::query query(host, service);
    endpoint_iterator = resolver.resolve(query);


	boost::asio::async_connect(conn.getSocket(), endpoint_iterator,
		boost::bind(&Client::handleConnect, this,
		boost::asio::placeholders::error));

	matrixQueue = MatrixQueue::getInstance();
	imageQueue = ImageQueue::getInstance();
}

void Client::handleConnect(const boost::system::error_code &e){
	if(!e){

		//mats.clear();
		logger::printTime("Try to Pop Matrix");

		SerializableMatrix m;
		matrixQueue->waitAndPop(m);
		mats.push_back(m);

		logger::printTime("Sending Matrix");

		conn.writeAsync(mats,
			boost::bind(&Client::handleSend, this,
			boost::asio::placeholders::error,
			std::size_t(1024)));
		
		logger::printTime("Begin read.");
		conn.readAsync(images,
			boost::bind(&Client::handleRead, this,
			boost::asio::placeholders::error));
	}
}


void Client::handleSend(const boost::system::error_code &e, std::size_t bytes_transferred){
	if(!e){
		logger::printTime("Mat send");
		mats.clear();
	}else{
		std::cerr << e.message() << std::endl;	
		boost::asio::async_connect(conn.getSocket(), endpoint_iterator,
		boost::bind(&Client::handleConnect, this,
		boost::asio::placeholders::error));
	}

}
void Client::handleRead(const boost::system::error_code &e){
	logger::printTime("End Read.");
	if(!e){
		for (int i = 0; i < images.size(); i++){
			std::cout << images[i].id << " length: " << images[i].image.size() << std::endl;
			imageQueue->push(images[i]);
		}
		images.clear();

		boost::asio::async_connect(conn.getSocket(), endpoint_iterator,
		boost::bind(&Client::handleConnect, this,
		boost::asio::placeholders::error));
	}
	else
    {
      // An error occurred.
      std::cerr << e.message() << std::endl;

	  conn.readAsync(images,
		boost::bind(&Client::handleRead, this,
		boost::asio::placeholders::error));
    }

}