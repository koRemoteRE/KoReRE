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
            
    }catch (exception e){
        cerr<<"Server Exception thrown: "<< e.what() <<endl;}
}