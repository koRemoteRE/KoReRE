//
//  CH264Decoder.cpp
//  ReReClient
//
//  Created by Ina Schröder on 07.05.13.
//
//

#include "CH264Decoder.h"

CH264Decoder::CH264Decoder(){
    //Register all formats and codecs once + initialize the network
    static std:: once_flag initFlag;
    std::call_once(
                   initFlag, [](){
                       av_register_all();
                       avformat_network_init();
                   });
};

void CH264Decoder::test(){
    std::cout<< "Test erfolgreich" <<std::endl;}