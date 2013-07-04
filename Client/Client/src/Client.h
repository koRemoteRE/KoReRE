#ifdef _WIN32 
# define _WIN32_WINNT 0x0501 
#endif 
#ifndef CLIENT
#define CLIENT

#include "Connection.hpp" // Must come before boost/serialization headers.
#include "MatrixQueue.h"
#include "ImageQueue.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <vector>
#include "SerializableMatrix.hpp"
#include "SerializableImage.hpp"
#include <boost/serialization/vector.hpp>

class Client
{
public:
	Client(boost::asio::io_service& io_service,
      const std::string& host, const std::string& service);

	void handleConnect(const boost::system::error_code &e);
	void handleSend(const boost::system::error_code &e, std::size_t bytes_transferred);
	void handleRead(const boost::system::error_code &e);

private:
	Connection conn;
	MatrixQueue *matrixQueue;
	ImageQueue *imageQueue;
	std::vector<SerializableMatrix> mats;
	std::vector<SerializableImage> images;
	boost::asio::ip::tcp::resolver::iterator endpoint_iterator;
};

#endif