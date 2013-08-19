#include "NoSerialServer.h"
#include "logger.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

NoSerialServer::NoSerialServer(unsigned short port_)
	:	port(port_),
		endpoint(boost::asio::ip::tcp::v4(), port_),
		socketAcceptor(io_service, endpoint), 
		sock(io_service){

		/*cv::Mat m2 = cv::Mat(4,4,CV_32F, glm::value_ptr(imPkt.matrix->mat));
		std::cout << "Bevor: " << m2 << std::endl;*/

		boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "9999"); 
		boost::asio::ip::tcp::resolver resolver(io_service); 
		boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query); 
		boost::asio::ip::tcp::resolver::iterator end; 
		while (it != end) 
		{ 
			std::cout << it->endpoint().address() << std::endl; 
			++it; 
		} 

		socketAcceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));

		socketAcceptor.listen();

		socketAcceptor.async_accept(sock, 
			boost::bind(&NoSerialServer::acceptHandler, this,
			boost::asio::placeholders::error));

		imageQueue = ImageQueue::getInstance();
		matrixQueue = MatrixQueue::getInstance();

		io_service.run();
}

NoSerialServer::~NoSerialServer(){}

void NoSerialServer::acceptHandler(const boost::system::error_code &ec){
	if(!ec){

		std::cout << "read" << std::endl;

		std::vector<char> *inHeader = new std::vector<char>(header_length);

		sock.async_read_some(boost::asio::buffer(*inHeader),
			boost::bind(&NoSerialServer::readHeaderHandler, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred,
			inHeader));
	}else{
		sock.close();
		socketAcceptor.async_accept(sock, 
			boost::bind(&NoSerialServer::acceptHandler, this,
			boost::asio::placeholders::error));
	}
}

void NoSerialServer::writeHandler(const boost::system::error_code &e, 
	std::size_t bytes_transferred,
	std::string *outHeader,
	std::string *outBuff){

	if(!e){
		//logger::printTime("Write Data end.");

		delete outHeader;
		delete outBuff;

		sock.close();
		socketAcceptor.async_accept(sock, 
			boost::bind(&NoSerialServer::acceptHandler, this,
			boost::asio::placeholders::error));

	}else{
		std::cout << e.message() << std::endl;

		sock.close();
		socketAcceptor.async_accept(sock, 
			boost::bind(&NoSerialServer::acceptHandler, this,
			boost::asio::placeholders::error));

	}

}

void NoSerialServer::readHeaderHandler(const boost::system::error_code &e,
									   std::size_t bytes_transferred,
									   std::vector<char> *inHeader){
	if(!e){
		std::stringstream headerStream;
		
		headerStream.write(inHeader->data(), header_length);
		delete inHeader;

		std::size_t inbound_data_size = 0;

		if(headerStream.str().size() > 0){
			headerStream >> std::hex >> inbound_data_size;
		}

		std::vector<char> *inBuff = new std::vector<char>(inbound_data_size);

		//multipass read
		std::stringstream *data = new std::stringstream();

		sock.async_read_some(boost::asio::buffer(*inBuff),
				boost::bind(&NoSerialServer::readHandler, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred,
				inBuff,
				data,
				inbound_data_size));
	}else{
		std::cerr << e.message() << std::endl;

		sock.close();
		socketAcceptor.async_accept(sock, 
			boost::bind(&NoSerialServer::acceptHandler, this,
			boost::asio::placeholders::error));
	}

}

void NoSerialServer::readHandler(const boost::system::error_code &e, 
								 std::size_t bytes_transferred, 
								 std::vector<char> *inBuff,
								 std::stringstream *data,
								 std::size_t inbound_data_size){
	if(!e || e == boost::asio::error::eof){
		
		if(bytes_transferred > 0){
			std::cout << bytes_transferred << std::endl;
		}

		data->write(inBuff->data(), bytes_transferred);

		inbound_data_size -= bytes_transferred;

		if(inbound_data_size > 0){
			inBuff->resize(inbound_data_size);

			sock.async_read_some(boost::asio::buffer(*inBuff),
				boost::bind(&NoSerialServer::readHandler, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred,
				inBuff,
				data,
				inbound_data_size));
		}else{
			
			delete inBuff;

			SerializableMatrix m;

			if(data->str().size() > 0){
                std::string stbuffer= data->str();
				m.deserialize(stbuffer);
				matrixQueue->push(m);

				//std::cout << m.mat[0][0] << std::endl;
			}

			delete data;

			/*********** WRITE ****************/

			std::cout << "wait and pop" << std::endl;

			SerializableImage img;
			imageQueue->waitAndPop(img);

			//serializing
			std::string *outBuff = new std::string();
			img.serializeInto(*outBuff);

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
				boost::bind(&NoSerialServer::writeHandler, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred,
				outHeader,
				outBuff));

		}
	}else{
		std::cerr << e.message() << std::endl;

		sock.close();
		socketAcceptor.async_accept(sock, 
			boost::bind(&NoSerialServer::acceptHandler, this,
			boost::asio::placeholders::error));
	}
}