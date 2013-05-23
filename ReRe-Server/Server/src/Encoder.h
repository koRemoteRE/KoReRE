#ifndef SERVER_ENCODER_H
#define SERVER_ENCODER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ConcurrentQueue.h"


extern "C" { 
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavformat\avio.h>
	#include <libavformat/rtp.h>
	#include <libavutil/mathematics.h>
	#include <libswscale/swscale.h>
}

class Encoder
{
public:
	ConcurrentQueue *queue;
  static Encoder* getInstance(){
    static Encoder instance; 
    return &instance;}
  ~Encoder(void);
 
  //bool init(char* Filename, int width, int height);
  bool init(ConcurrentQueue *q, int width, int height);
  void encodeFrame();
  AVPacket* getCurrentPacket();
  void finish();
  void start();
  void stop();

private:
  Encoder(void);

  AVStream *add_stream(AVFormatContext *oc, AVCodec **codec, enum AVCodecID codec_id);
  void open_video(AVFormatContext *oc, AVCodec *codec, AVStream *st);
 
  AVPacket* currPacket;
  uint8_t* buffer;
  glm::ivec2 renderResolution;
  GLenum 	lastBuffer;

  AVCodec* codec;
  AVCodecContext* codecContext;
  AVFormatContext* rtpContext;
 
  AVOutputFormat* fmt;
  AVFormatContext* formatContext;
  AVIOContext *ioContext;

  AVStream *videoStream;
  AVStream *actualStream;
  AVStream *rtpStream;
  AVFrame *frame;
  AVPicture src_picture, dst_picture;
  int frame_count;
  int ret;

  AVFrame* picRGB;
  AVFrame* picYUV;
  struct SwsContext* sws_context;

  FILE* file;
  uint8_t* outbuf;
  uint8_t* picYUVbuf;
  uint8_t* destBuffer;
  int outbuf_size;
  int out_size;
  int size;
  int frameno;

  double video_pts;

  bool _recording;
  bool _initialized;
};
#endif //SERVER_ENCODER_H
