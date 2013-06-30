#ifndef SERIALIZATION_CONNECTION
#define SERIALIZATION_CONNECTION

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

class Connection
{
public:
	Connection(boost::asio::io_service &io_service);
	~Connection(void);

	boost::asio::ip::tcp::socket &getSocket(){
		return socket;
	}

	template <typename T, typename Handler>
	void writeAsync(const T &t, Handler handler);

	template <typename T, typename Handler>
	void readAsync(T &t, Handler handler);

	template <typename T, typename Handler>
	void handleReadHeader(const boost::system::error_code &e,
		T &t, boost::tuple<Handler> handler);

	template <typename T, typename Handler>
	void handleReadData(const boost::system::error_code &e,
		T &t, boost::tuple<Handler> handler);

private:
	boost::asio::ip::tcp::socket socket;
	enum {header_length = 8};

	std::string outbound_header;
	std::string outbound_data;

	char inbound_header[header_length];

	std::vector<char> inbound_data;
};

typedef boost::shared_ptr<Connection> connection_ptr;

#endif