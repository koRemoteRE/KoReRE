#ifdef _WIN32 
# define _WIN32_WINNT 0x0501 
#endif 
#ifndef ON_DEMAND_SERVER
#define ON_DEMAND_SERVER


#include "ImageQueue.h"
#include "MatrixQueue.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>

class NoSerialServer{
private:
	unsigned short port;
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::endpoint endpoint;
	boost::asio::ip::tcp::acceptor socketAcceptor;
	boost::asio::ip::tcp::socket sock;

	ImageQueue *imageQueue;
	MatrixQueue *matrixQueue;
	
public:
	NoSerialServer(unsigned short port_);
	~NoSerialServer(void);

	void acceptHandler(const boost::system::error_code &ec);
	void writeHandler(const boost::system::error_code &ec, 
		std::size_t bytes_transferred);
	void readHandler(const boost::system::error_code &e, 
		std::size_t bytes_transferred);

	std::queue<boost::array<char, 1024>> buffer;
	std::queue<std::string> outQueue;
};
#endif

