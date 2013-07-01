#include "Client.h"


Client::Client(boost::asio::io_service& io_service,
               const std::string& host, const std::string& service)
: connection(io_service)
{
	boost::asio::ip::tcp::resolver resolver(io_service);
	boost::asio::ip::tcp::resolver::query query(host, service);
    endpoint_iterator = resolver.resolve(query);
    
	boost::asio::async_connect(connection.getSocket(), endpoint_iterator,
                               boost::bind(&Client::handleConnect, this,
                                           boost::asio::placeholders::error));
    
	matrixQueue = MatrixQueue::getInstance();
	imageQueue = ImageQueue::getInstance();
}

void Client::handleConnect(const boost::system::error_code &e){
	if(!e){
        
		//mats.clear();
		SerializableMatrix m = {0};
		matrixQueue->waitAndPop(m);
		mats.push_back(m);
        
		connection.writeAsync(mats,
                              boost::bind(&Client::handleSend, this,
                                          boost::asio::placeholders::error,
                                          std::size_t(1024)));
	}
}


void Client::handleSend(const boost::system::error_code &e, std::size_t bytes_transferred){
	if(!e){
		mats.clear();
	}else{
		std::cerr << e.message() << std::endl;
		boost::asio::async_connect(connection.getSocket(), endpoint_iterator,
                                   boost::bind(&Client::handleConnect, this,
                                               boost::asio::placeholders::error));
	}
	connection.readAsync(images,
                         boost::bind(&Client::handleRead, this,
                                     boost::asio::placeholders::error));
    
	
}
void Client::handleRead(const boost::system::error_code &e){
	if(!e){
		for (int i = 0; i < images.size(); i++){
			std::cout << images[i].id << " length: " << images[i].len << std::endl;
			imageQueue->push(images[i]);
		}
		images.clear();
        
        
		boost::asio::async_connect(connection.getSocket(), endpoint_iterator,
                                   boost::bind(&Client::handleConnect, this,
                                               boost::asio::placeholders::error));
	}
	else
    {
        // An error occurred.
        std::cerr << e.message() << std::endl;
        
        connection.readAsync(images,
                             boost::bind(&Client::handleRead, this,
                                         boost::asio::placeholders::error));
    }
    
}