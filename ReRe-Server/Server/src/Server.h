#ifdef _WIN32 
# define _WIN32_WINNT 0x0501 
#endif 
#ifndef ON_DEMAND_SERVER
#define ON_DEMAND_SERVER


#include "ImageQueue.h"
#include "MatrixQueue.h"
#include "Connection.h"
#include <boost\asio.hpp>
#include <boost\bind.hpp>


class Server{
private:
	ImageQueue *imageQueue;
	MatrixQueue *matrixQueue;

	boost::asio::ip::tcp::acceptor socketAcceptor;
public:
	Server(boost::asio::io_service &io_service, unsigned short port);
	~Server(void);

	void init();
	void start();

	void acceptHandler(const boost::system::error_code &e, connection_ptr conn);
	void writeHandler(const boost::system::error_code &ec, std::size_t bytes_transferred);
};
#endif