//
//  Packet.h
//  ReReClient
//
//  Created by Ina Schröder on 07.05.13.
//
//
//inlcude
extern "C" {
#include <libav/libavcodec/avcodec.h>
#include <libav/libavformat/avformat.h>
}


#ifndef ReReClient_Packet_h
#define ReReClient_Packet_h

struct Packet {
	explicit Packet(AVFormatContext* ctxt = nullptr) {
		av_init_packet(&packet);
		packet.data = nullptr;
		if (ctxt) reset(ctxt);
	}
    
	Packet(Packet&& other) : packet(std::move(other.packet)) {
		other.packet.data = nullptr;
	}
    
	~Packet() {
		if (packet.data)
			av_free_packet(&packet);
	}
    
	void reset(AVFormatContext* ctxt) {
		if (packet.data)
			av_free_packet(&packet);
		if (av_read_frame(ctxt, &packet) < 0)
			packet.data = nullptr;
	}
    
	AVPacket packet;
};

#endif
