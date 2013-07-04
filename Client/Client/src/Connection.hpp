#ifdef _WIN32 
# define _WIN32_WINNT 0x0501 
#endif 
#ifndef SERIALIZATION_CONNECTION
#define SERIALIZATION_CONNECTION

#include "SerializableMatrix.hpp"
#include "SerializableImage.hpp"
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
#include "logger.h"



class Connection
{
public:
	Connection(boost::asio::io_service &io_service)
		:socket(io_service){}

	boost::asio::ip::tcp::socket &getSocket(){
		return socket;
	}

	template <typename T, typename Handler>
	void writeAsync(const T &t, Handler handler){
	
		//Serialize
		std::ostringstream archiveStream;
		boost::archive::text_oarchive archive(archiveStream);
		archive << t;
		outbound_data = archiveStream.str();

		//Header format
		std::ostringstream headerStream;
		headerStream << std::setw(header_length) << std::hex << outbound_data.size();
		if(!headerStream || headerStream.str().size() != header_length){
			boost::system::error_code error(boost::asio::error::invalid_argument);
			socket.get_io_service().post(boost::bind(handler, error));
			return;
		}

		outbound_header = headerStream.str();

		//Write Data to Socket
		std::vector<boost::asio::const_buffer> buffers;
		buffers.push_back(boost::asio::buffer(outbound_header));
		buffers.push_back(boost::asio::buffer(outbound_data));
		boost::asio::async_write(socket, buffers, handler);

	}

	template <typename T, typename Handler>
	void readAsync(T &t, Handler handler){

		//Read exactly the number of bytes in a header
		void (Connection::*f)(const boost::system::error_code&, T&, boost::tuple<Handler>)
			= &Connection::handleReadHeader<T, Handler>;


		boost::asio::async_read(socket, boost::asio::buffer(inbound_header),
			boost::bind(f, this, boost::asio::placeholders::error, boost::ref(t),
			boost::make_tuple(handler)));
	}

	template <typename T, typename Handler>
	void handleReadHeader(const boost::system::error_code &e,
		T &t, boost::tuple<Handler> handler){
			logger::printTime("Read header.");
		if(e){
			boost::get<0>(handler)(e);
		}else{
			std::istringstream is(std::string(inbound_header, header_length));
			std::size_t inbound_data_size = 0;
			if(!(is >> std::hex >> inbound_data_size)){
				boost::system::error_code error(boost::asio::error::invalid_argument);
				boost::get<0>(handler)(error);
				return;
			}

			inbound_data.resize(inbound_data_size);
			void (Connection::*f)(const boost::system::error_code&, T&, boost::tuple<Handler>)
				= &Connection::handleReadData<T, Handler>;
			boost::asio::async_read(socket, boost::asio::buffer(inbound_data),
				boost::bind(f, this,
				boost::asio::placeholders::error, boost::ref(t), handler));
		}
	}

	template <typename T, typename Handler>
	void handleReadData(const boost::system::error_code &e,
		T &t, boost::tuple<Handler> handler){
			logger::printTime("Read Data.");
			if (e)
		{
		  boost::get<0>(handler)(e);
		}
		else
		{
		  // Extract the data structure from the data just received.
		  try
		  {
			std::string archive_data(&inbound_data[0], inbound_data.size());
			std::istringstream archive_stream(archive_data);
			boost::archive::text_iarchive archive(archive_stream);
			archive >> t;
		  }
		  catch (std::exception& ex)
		  {
			  std::cout << ex.what() << std::endl;
			// Unable to decode data.
			boost::system::error_code error(boost::asio::error::invalid_argument);
			boost::get<0>(handler)(error);
			return;
		  }

		  // Inform caller that data has been received ok.
		  boost::get<0>(handler)(e);
		}
	}

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