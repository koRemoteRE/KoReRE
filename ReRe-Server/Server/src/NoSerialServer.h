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

	enum {header_length = 16};
	
public:
	NoSerialServer(unsigned short port_);
	~NoSerialServer(void);

	/*1. Connectin accepted -> read HeaderData to get Size of sent Data*/
	void acceptHandler(const boost::system::error_code &ec);

	/* 2. Read Header data to get size of sent Data. 
	success -> read sent Data in multiple pases*/
	void readHeaderHandler(const boost::system::error_code &e,
		std::size_t bytes_transferred,
		std::vector<char> *inHeader);

	/* 3. read sent Data packets in multiple passes.
 	success: switch to write mode and wait for a ready image*/
	void readHandler(const boost::system::error_code &e, 
		std::size_t bytes_transferred,
		std::vector<char> *inBuff,
		std::stringstream *data,
		std::size_t inbound_data_size);

	/* 4. If Data written to socket close connection and open another.*/
	void writeHandler(const boost::system::error_code &ec, 
		std::size_t bytes_transferred,
		std::string *outHeader,
		std::string *outBuff);
};
#endif

