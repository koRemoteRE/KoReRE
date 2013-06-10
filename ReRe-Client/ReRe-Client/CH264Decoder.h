//
//  CH264Decoder.h
//  ReReClient
//
//  Created by Ina Schröder on 07.05.13.
//
//

#ifndef __ReReClient__CH264Decoder__
#define __ReReClient__CH264Decoder__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <thread>
#include <mutex>
#include <vector>
#include <array>

//inlcude
extern "C" {
#include <libav/libavcodec/avcodec.h>
#include <libav/libavformat/avformat.h>
#include <libav/libswscale/swscale.h>
#include <libav/libavutil/log.h>
#include <libav/libavutil/avutil.h>
}


class CH264Decoder
{
public:
CH264Decoder();
~CH264Decoder();
    
    
void test();

};
#endif /* defined(__ReReClient__CH264Decoder__) */
