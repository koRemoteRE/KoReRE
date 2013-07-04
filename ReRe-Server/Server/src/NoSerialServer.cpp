#include "NoSerialServer.h"
#include "logger.h"

boost::array<char, 1024> buffer;
//std::vector<float> buffer;
std::string outBuffer;

//typedef boost::shared_ptr<std::string> outBufPointer;

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
		sock.async_read_some(boost::asio::buffer(buffer),
			boost::bind(&NoSerialServer::readHandler, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}
}

void NoSerialServer::writeHandler(const boost::system::error_code &e, 
						  std::size_t bytes_transferred){
	if(!e){
		logger::printTime("Write Data end.");
		//images.clear();
	}else{
		std::cout << e.message() << std::endl;
	}

	/*connection_ptr new_conn(new Connection(socketAcceptor.get_io_service()));

		socketAcceptor.async_accept(new_conn->getSocket(),
			boost::bind(&NoSerialServer::acceptHandler, this, 
			boost::asio::placeholders::error, new_conn));*/
}

void NoSerialServer::readHandler(const boost::system::error_code &e, std::size_t bytes_transferred){
	if(!e){
		SerializableMatrix m;
		std::stringstream data(buffer.data(), bytes_transferred);
		if(data){
			m.deserialize(data.str());

			matrixQueue->push(m);

			//std::cout << m.mat[0][0] << std::endl;
		}

		if(bytes_transferred > 0){
			std::cout << bytes_transferred << std::endl;
		}

		sock.async_read_some(boost::asio::buffer(buffer),
			boost::bind(&NoSerialServer::readHandler, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

		SerializableImage img;
		imageQueue->waitAndPop(img);

		outBuffer = *img.serialize();
		
		//outBufPointer outBuffer(new std::string(*img->serialize()));
		
		sock.async_write_some(boost::asio::buffer(outBuffer),
			boost::bind(&NoSerialServer::writeHandler, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

	}else{

      std::cerr << e.message() << std::endl;
	}
}