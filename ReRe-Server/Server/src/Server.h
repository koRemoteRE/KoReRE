#ifdef _WIN32 
# define _WIN32_WINNT 0x0501 
#endif 
#ifndef ON_DEMAND_SERVER
#define ON_DEMAND_SERVER


#include "ImageQueue.h"
#include "MatrixQueue.h"
#include <boost\asio.hpp>

class Server{
private:
	ImageQueue *imageQueue;
	MatrixQueue *matrixQueue;
public:
	Server(void);
	~Server(void);

	void init();
	void start();

	void acceptHandler(const boost::system::error_code &ec);
	void writeHandler(const boost::system::error_code &ec, std::size_t bytes_transferred);
};
#endif