//
//  RemoteNetwork.h
//  ReRe-Client
//
//  Created by Thomas Kipshagen on 23.04.13.
//  Copyright (c) 2013 Thomas Kipshagen. All rights reserved.
//

#ifndef __ReRe_Client__RemoteNetwork__
#define __ReRe_Client__RemoteNetwork__

#include <iostream>
#include <sstream>

#include <boost/asio.hpp>
#include <boost/array.hpp>
//#include <opencv2/core/core.hpp>


class CRemoteNetwork
{
    
public:
    CRemoteNetwork();
    ~CRemoteNetwork();
    
    void sendCameraMatrix();
    void sendPerspMatrix();
};

#endif /* defined(__ReRe_Client__RemoteNetwork__) */
