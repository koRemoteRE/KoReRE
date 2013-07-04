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
		boost::array<char, 1024> temp;
		buffer.push(temp);
		sock.async_read_some(boost::asio::buffer(buffer.back()),
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
		//sock.shutdown(sock.shutdown_both);
		////sock.close();
	 //   //sock = boost::asio::ip::tcp::socket(io_service);
		//io_service.reset();
		//io_service.run();
		//socketAcceptor.async_accept(sock, 
		//	boost::bind(&NoSerialServer::acceptHandler, this,
		//	boost::asio::placeholders::error));
	}

	/*connection_ptr new_conn(new Connection(socketAcceptor.get_io_service()));

		socketAcceptor.async_accept(new_conn->getSocket(),
			boost::bind(&NoSerialServer::acceptHandler, this, 
			boost::asio::placeholders::error, new_conn));*/
}

void NoSerialServer::readHandler(const boost::system::error_code &e, std::size_t bytes_transferred){
	if(!e){
		SerializableMatrix m;
		std::stringstream data;//(buffer.front().data(), bytes_transferred);
		data.write(buffer.front().data(), bytes_transferred);
		buffer.pop();
		if(data){
			m.deserialize(data.str());

			matrixQueue->push(m);

			//std::cout << m.mat[0][0] << std::endl;
		}

		if(bytes_transferred > 0){
			std::cout << bytes_transferred << std::endl;
		}
		boost::array<char, 1024> temp;
		buffer.push(temp);
		sock.async_read_some(boost::asio::buffer(buffer.back()),
			boost::bind(&NoSerialServer::readHandler, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

		SerializableImage img;
		imageQueue->waitAndPop(img);

		//outBuffer = *img.serialize();
		outQueue.push(*img.serialize());
		
		sock.async_write_some(boost::asio::buffer(outQueue.back()),
			boost::bind(&NoSerialServer::writeHandler, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
		if(outQueue.size() > 10){
			outQueue.pop();
		}
		
	}else{
      std::cerr << e.message() << std::endl;
	  //sock.shutdown(sock.shutdown_both);
	  ////sock.close();
	  ////sock = boost::asio::ip::tcp::socket(io_service);
	  //io_service.reset();
	  //io_service.run();
	  //socketAcceptor.async_accept(sock, 
			//boost::bind(&NoSerialServer::acceptHandler, this,
			//boost::asio::placeholders::error));
	}
}