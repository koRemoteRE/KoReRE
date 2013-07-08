#ifdef _WIN32 
# define _WIN32_WINNT 0x0501 
#endif 

#ifndef NO_SERIAL_CLIENT
#define NO_SERIAL_CLIENT

#include "ImageQueue.h"
#include "MatrixQueue.h"

#include <string>
#include <vector>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>


class NoSerialClient
{
public:
	NoSerialClient(const std::string host, const std::string port);
	~NoSerialClient(void);

	void resolveHandler(const boost::system::error_code &ec, 
		boost::asio::ip::tcp::resolver::iterator it);

	void connectHandler(const boost::system::error_code &ec);
	void writeHandler(const boost::system::error_code &ec, std::size_t bytesTransferred);
	void readHandler(const boost::system::error_code &ec, 
								 std::size_t bytesTransferred);

private:
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::resolver resolver;
	boost::asio::ip::tcp::socket sock;
	boost::asio::ip::tcp::resolver::iterator endpointIterator;
	boost::asio::ip::tcp::resolver::query query;

	ImageQueue *imageQueue;
	MatrixQueue *matrixQueue;
};

#endif
