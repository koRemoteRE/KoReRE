#include "NoSerialServer.h"
#include "logger.h"

NoSerialServer::NoSerialServer(unsigned short port_)
	:	port(port_),
		endpoint(boost::asio::ip::tcp::v4(), port_),
		socketAcceptor(io_service, endpoint), 
		sock(io_service){

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

		std::vector<char> *inBuff = new std::vector<char>(1024);

		sock.async_read_some(boost::asio::buffer(*inBuff),
			boost::bind(&NoSerialServer::readHandler, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred,
			inBuff));
	}
}

void NoSerialServer::writeHandler(const boost::system::error_code &e, 
						  std::size_t bytes_transferred){
	if(!e){
		logger::printTime("Write Data end.");

	}else{
		std::cout << e.message() << std::endl;

	}

}

void NoSerialServer::readHandler(const boost::system::error_code &e, 
								 std::size_t bytes_transferred, 
								 std::vector<char> *inBuff){
	if(!e){
		SerializableMatrix m;
		std::stringstream data;

		data.write(inBuff->data(), bytes_transferred);


		delete inBuff;

		if(data){
			m.deserialize(data.str());

			matrixQueue->push(m);

			//std::cout << m.mat[0][0] << std::endl;
		}

		if(bytes_transferred > 0){
			std::cout << bytes_transferred << std::endl;
		}

		std::cout << "read" << std::endl;

		std::vector<char> *inBuff = new std::vector<char>(1024);

		sock.async_read_some(boost::asio::buffer(*inBuff),
			boost::bind(&NoSerialServer::readHandler, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred,
			inBuff));

		std::cout << "wait and pop" << std::endl;

		SerializableImage img;
		imageQueue->waitAndPop(img);

		std::cout << "popped" << std::endl;

		std::string *outBuff = new std::string();
		img.serializeInto(*outBuff);
		
		sock.async_write_some(boost::asio::buffer(*outBuff),
			boost::bind(&NoSerialServer::writeHandler, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
		
		
	}else{
      std::cerr << e.message() << std::endl;
	}
}