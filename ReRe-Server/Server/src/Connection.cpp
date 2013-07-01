Connection::Connection(boost::asio::io_service &io_service)
	: socket(io_service){}


Connection::~Connection(void){}

template <typename T, typename Handler>
void Connection::writeAsync(const T &t, Handler handler){
	
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
void Connection::readAsync(T &t, Handler handler){
	//Read exactly the number of bytes in a header
	void (Connection::*f)(const boost::system::error_code&, T&, boost::tuple<Handler>)
		= &Connection::handleReadHeader<T, Handler>;

	boost::asio::async_read(socket, boost::asio::buffer(inbound_header),
		boost::bind(f, this, boost::asio::placeholders::error, boost::ref(t),
		boost::make_tuple(handler)));
}

template <typename T, typename Handler>
void Connection::handleReadHeader(const boost::system::error_code &e,
								  T &t, boost::tuple<Handler> handler){
	if(e){
		boost::get<0>(handler)(e);
	}else{
		std::istringstream is(std::string(inbound_header, header_length));
		std::size_t inbouninbound_data_size = 0;
		if(!(is >> std::hex >> inbouninbound_data_size)){
			boost::system::error_code error(boost::asio::error::invalid_argument);
			boost::get<0>(handler)(error);
			return;
		}

		inbound_data.resize(inbouninbound_data_size);
		void (Connection::*f)(const boost::system::error_code&, T&, boost::tuple<Handler>)
			= &Connection::handleReadData<T, Handler>;
		boost::asio::async_read(socket, boost::asio::buffer(inbound_data),
			boost::bind(f, this,
			boost::asio::placeholders::error, boost::ref(t), handler));
	}
}

template <typename T, typename Handler>
void Connection::handleReadData(const boost::system::error_code &e,
								T &t, boost::tuple<Handler> handler){
	if(e){
		boost::get<0>(handler)(e);
	}else{
		try{
			std::string archiveData(&inbound_data[0], inbound_data.size());
			std::istringstream archiveStream(archiveData);
			boost::archive::text_iarchive archive(archiveStream);
			archive >> t;
		}catch(std::exception &e){
			boost::system::error_code error(boost::asio::error::invalid_argument);
			boost::get<0>(handler)(error);
			return;
		}

		boost::get<0>(handler)(e);
	}
}

