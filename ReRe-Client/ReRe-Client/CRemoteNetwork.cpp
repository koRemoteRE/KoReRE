//
//  RemoteNetwork.cpp
//  ReRe-Client
//
//  Created by Thomas Kipshagen on 23.04.13.
//  Copyright (c) 2013 Thomas Kipshagen. All rights reserved.
//

#include "CRemoteNetwork.h"


using namespace std;
using namespace boost::asio;


CRemoteNetwork::CRemoteNetwork() {
    
    try{
        //open connection
        io_service io_service;
        
        ip::tcp::resolver resolver(io_service);
    
        ip::tcp::resolver::query query("141.26.88.55","12345");
        ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        
        ip::tcp::socket socket(io_service);
        connect(socket, endpoint_iterator);
        
        //send matrix
        boost::array<char,16> matrixBuf;
        for (int i=0; i<16; i++)
            matrixBuf[i]=i;
        
        cout << "Sending data..." << endl;
        socket.send(buffer(matrixBuf));
        cout << "... finish" << endl;
        
        //http://stackoverflow.com/questions/5126923/boost-asio-send-opencv-iplimage-from-ubuntu-server-to-win7-client

        string header;
        stringstream ss;

        cout << "Reading data..." << endl;

        for(;;){
        	boost::array<char, 1024> picBuffer;
        	boost::system::error_code error;
        	//will block until read some data
        	size_t len = socket.read_some(buffer(picBuffer),error);
        	if(error == error::eof){
        		break; //finished reading
        	}
        	else if(error){
        		throw boost::system::system_error(error);
        	}

        	ss.write(picBuffer.data(),len);

        }
        header = ss.str();
        cout << "...finish! Size: " << header.size() << endl;
            
    }catch (exception e){
        cerr<<"Server Exception thrown: "<< e.what() <<endl;}
}


void CRemoteNetwork::sendCameraMatrix()
{


}
