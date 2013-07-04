#include "NoSerialClient.h"

//std::vector<float> outBuffer;

//std::stringstream inString;
boost::array<char, 2097152> inBuffer;
std::queue<std::string> outBuffer;

NoSerialClient::NoSerialClient(const std::string host, const std::string port)
:
resolver(io_service),
sock(io_service),
query(host, port)
{
    std::cout << "resolving query" << std::endl;
    resolver.async_resolve(query,
                           boost::bind(&NoSerialClient::resolveHandler, this,
                                       boost::asio::placeholders::error,
                                       boost::asio::placeholders::iterator));
    
    imageQueue = ImageQueue::getInstance();
    matrixQueue = MatrixQueue::getInstance();
    
    
    io_service.run();
}


NoSerialClient::~NoSerialClient(void)
{
}

void NoSerialClient::resolveHandler(const boost::system::error_code &ec,
                                    boost::asio::ip::tcp::resolver::iterator it){
    if(!ec){
        std::cout << "resolving done...connecting" << std::endl;
        sock.async_connect(*it,
                           boost::bind(&NoSerialClient::connectHandler, this,
                                       boost::asio::placeholders::error));
    }else{
        std::cerr << "resolveHandler error: " << ec << std::endl;
    }
}

void NoSerialClient::connectHandler(const boost::system::error_code &ec){
    if(!ec){
        std::cout << "connected" << std::endl;
        SerializableMatrix m;
        matrixQueue->waitAndPop(m);
        
        outBuffer.push(m.serialize());
        
        sock.async_write_some(boost::asio::buffer(outBuffer.back()),
                              boost::bind(&NoSerialClient::writeHandler, this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
        
        
        
        sock.async_read_some(boost::asio::buffer(inBuffer),
                             boost::bind(&NoSerialClient::readHandler, this,
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
    }else{
        std::cerr << "connectHandler error: " << ec << std::endl;
    }
}

void NoSerialClient::writeHandler(const boost::system::error_code &ec,
                                  std::size_t bytesTransferred){
    if(!ec){
        //outBuffer.clear();
        
        SerializableMatrix m;
        matrixQueue->waitAndPop(m);
        
        outBuffer.push(m.serialize());
        
        sock.async_write_some(boost::asio::buffer(outBuffer.back()),
                              boost::bind(&NoSerialClient::writeHandler, this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
        
        
        if(outBuffer.size() > 10){
            outBuffer.pop();
        }
        
        std::cout << bytesTransferred << std::endl;
    }else{
        std::cerr << "writeHandler error: " << ec << std::endl;
    }
}

void NoSerialClient::readHandler(const boost::system::error_code &ec,
                                 std::size_t bytesTransferred){
    if(!ec){
        SerializableImage img;
        
        //std::stringstream in(inBuffer.data(), bytesTransferred);
        std::stringstream inString;
        
        inString.write(inBuffer.data(), bytesTransferred);
        
        if(inString.str().size() > 0){
            std::string buffer = inString.str();
            img.deserialize(buffer);
            
            imageQueue->push(img);
            
            /*std::cout << "bytes:" << bytesTransferred << std::endl;
             std::cout << "InStr Size:" << inString.str().size() << std::endl;*/
            std::cout << "Image Size:" << img.image->size() << std::endl;
        }
        
        sock.async_read_some(boost::asio::buffer(inBuffer),
                             boost::bind(&NoSerialClient::readHandler, this,
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
        
        //inString.str("");
        
    }else{
        if(ec == boost::asio::error::eof){
            
        }
    }
}