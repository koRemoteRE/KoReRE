#include "NoSerialClient.h"

NoSerialClient::NoSerialClient(boost::asio::io_service &io_service, const std::string host, const std::string port)
	:
	resolver(io_service),
	sock(io_service),
	query(host, port)
{
	//std::cout << "resolving query" << std::endl;
	resolver.async_resolve(query,
		boost::bind(&NoSerialClient::resolveHandler, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::iterator));

	imageQueue = ImageQueue::getInstance();
	matrixQueue = MatrixQueue::getInstance();

	io_service.run();

}


NoSerialClient::~NoSerialClient(void){}

void NoSerialClient::resolveHandler(const boost::system::error_code &ec, 
									boost::asio::ip::tcp::resolver::iterator it){
	if(!ec){
		//std::cout << "resolving done...connecting" << std::endl;
		sock.async_connect(*it, 
			boost::bind(&NoSerialClient::connectHandler, this,
			boost::asio::placeholders::error));
	}else{
		std::cerr << "resolveHandler error: " << ec << std::endl;
	}
}

void NoSerialClient::connectHandler(const boost::system::error_code &ec){
	if(!ec){
		//std::cout << "connected" << std::endl;

		SerializableMatrix m;
		matrixQueue->waitAndPop(m);

		//serializing
		std::string *outBuff = new std::string();
		m.serializeInto(*outBuff);

		//write header
		std::ostringstream headerStream;
		std::string *outHeader = new std::string();
		headerStream << std::setw(header_length) << std::hex << outBuff->size();
		*outHeader = headerStream.str();

		//merge header and buffer
		std::vector<boost::asio::const_buffer> buffers;
		buffers.push_back(boost::asio::buffer(*outHeader));
		buffers.push_back(boost::asio::buffer(*outBuff));

		//send data
		sock.async_write_some(buffers,
			boost::bind(&NoSerialClient::writeHandler, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred,
			outHeader,
			outBuff));

		std::cout << "send mat" << std::endl;

	}else{
		std::cerr << "connectHandler error: " << ec << std::endl;

	}
}


void NoSerialClient::writeHandler(const boost::system::error_code &ec, 
								  std::size_t bytesTransferred,
								  std::string *outHeader,
								  std::string *outBuff){
	if(!ec){

		delete outHeader;
		delete outBuff;

		std::cout << "read" << std::endl;

		std::vector<char> *inHeader = new std::vector<char>(header_length);

		sock.async_read_some(boost::asio::buffer(*inHeader), 
			boost::bind(&NoSerialClient::readHeaderHandler, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred,
			inHeader));		

	}else{
		std::cerr << "writeHandler error: " << ec << std::endl;

		clients c(new NoSerialClient(resolver.get_io_service(), query.host_name(), query.service_name()));
	}
}

void NoSerialClient::readHeaderHandler(const boost::system::error_code &ec, 
								 std::size_t bytesTransferred,
								 std::vector<char> *inHeader){
	if(!ec){

		std::stringstream headerStream;
		headerStream.write(inHeader->data(), header_length);
		std::size_t inbound_data_size = 0;

		if(headerStream.str().size() > 0){
			headerStream >> std::hex >> inbound_data_size;
		}

		delete inHeader;

		std::cout << "read" << std::endl;
		std::vector<char> *inBuff = new std::vector<char>(inbound_data_size);

		//multipass read
		std::stringstream *data = new std::stringstream();

		sock.async_read_some(boost::asio::buffer(*inBuff), 
			boost::bind(&NoSerialClient::readHandler, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred,
			inBuff, 
			data,
			inbound_data_size));		

	}else{
		std::cerr << "writeHandler error: " << ec << std::endl;
		
		clients c(new NoSerialClient(resolver.get_io_service(), query.host_name(), query.service_name()));
	}
}

void NoSerialClient::readHandler(const boost::system::error_code &ec, 
								 std::size_t bytesTransferred,
								 std::vector<char> *inBuff,
								 std::stringstream *data,
								 std::size_t inbound_data_size){

		if(!ec || ec == boost::asio::error::eof){
		
		data->write(inBuff->data(), bytesTransferred);

		inbound_data_size -= bytesTransferred;

		if(inbound_data_size > 0){
			inBuff->resize(inbound_data_size);

			//std::cout << "schritt " << std::flush;

			sock.async_read_some(boost::asio::buffer(*inBuff), 
				boost::bind(&NoSerialClient::readHandler, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred,
				inBuff, 
				data,
				inbound_data_size));	

		}else{
			
			delete inBuff;

			SerializableImage img;

			if(data->str().size() > 0){
                std::string strBuffer = data->str();
				img.deserialize(strBuffer);

				imageQueue->push(img);

				//std::cout << "InStr Size:" << inString.str().size() << std::endl;
				std::cout << "Image Size:" << img.image->size() << std::flush;
			}

			delete data;

			clients c(new NoSerialClient(resolver.get_io_service(), query.host_name(), query.service_name()));

		}

	}else{

		clients c(new NoSerialClient(resolver.get_io_service(), query.host_name(), query.service_name()));

	}
}
