#include "NoSerialClient.h"

NoSerialClient::NoSerialClient(const std::string host, const std::string port)
	:
	resolver(io_service),
	sock(io_service),
	query(host, port)
{
	std::cout << "resolving query" << std::endl;
	resolver.async_resolve(query,
		boost::bind(&NoSerialClient::resolveHandler, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::iterator));

	imageQueue = ImageQueue::getInstance();
	matrixQueue = MatrixQueue::getInstance();


	io_service.run();
}


NoSerialClient::~NoSerialClient(void)
{
}

void NoSerialClient::resolveHandler(const boost::system::error_code &ec, 
									boost::asio::ip::tcp::resolver::iterator it){
	if(!ec){
		std::cout << "resolving done...connecting" << std::endl;
		sock.async_connect(*it, 
			boost::bind(&NoSerialClient::connectHandler, this,
			boost::asio::placeholders::error));
	}else{
		std::cerr << "resolveHandler error: " << ec << std::endl;
	}
}

void NoSerialClient::connectHandler(const boost::system::error_code &ec){
	if(!ec){
		std::cout << "connected" << std::endl;

		SerializableMatrix m;
		matrixQueue->waitAndPop(m);

		std::string *outBuff = new std::string();
		m.serializeInto(*outBuff);

		sock.async_write_some(boost::asio::buffer(*outBuff),
			boost::bind(&NoSerialClient::writeHandler, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

		std::cout << "send mat" << std::endl;

	}else{
		std::cerr << "connectHandler error: " << ec << std::endl;
	}
}

void NoSerialClient::writeHandler(const boost::system::error_code &ec, 
								  std::size_t bytesTransferred){
	if(!ec){

		std::cout << "read" << std::endl;

		std::vector<char> *inBuff = new std::vector<char>(2097152);

		sock.async_read_some(boost::asio::buffer(*inBuff), 
			boost::bind(&NoSerialClient::readHandler, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred,
			inBuff));		

	}else{
		std::cerr << "writeHandler error: " << ec << std::endl;
	}
}

void NoSerialClient::readHandler(const boost::system::error_code &ec, 
								 std::size_t bytesTransferred,
								 std::vector<char> *inBuff){
	if(!ec){
		
		std::stringstream data;
		data.write(inBuff->data(), bytesTransferred);

		delete inBuff;

		SerializableImage img;

		if(data.str().size() > 0){
			img.deserialize(data.str());

			imageQueue->push(img);

			//std::cout << "InStr Size:" << inString.str().size() << std::endl;
			std::cout << "Image Size:" << img.image->size() << std::flush;
		}

		SerializableMatrix m;
		std::cout << "try pop" << std::endl;

		matrixQueue->waitAndPop(m);

		std::cout << "popped" << std::endl;

		std::string *outBuff = new std::string();
		m.serializeInto(*outBuff);

		sock.async_write_some(boost::asio::buffer(*outBuff),
			boost::bind(&NoSerialClient::writeHandler, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

		std::cout << bytesTransferred << std::endl;
	}else{
		if(ec == boost::asio::error::eof){
			
		}
	}
}
