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
#include <libavutil/mathematics.h>
#include <libswscale/swscale.h>
}

class Encoder
{
public:
  static Encoder* getInstance(){
    static Encoder instance; 
    return &instance;}
  ~Encoder(void);
 
  bool init(char* Filename, int width, int height);
  void encodeFrame(uint8_t ID);
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
 
  AVOutputFormat* fmt;
  AVFormatContext* fc;
  AVStream *videoStream;
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
  int outbuf_size;
  int out_size;
  int size;
  int frameno;
  bool frameReadyForStream;

  double video_pts;

  bool _recording;
  bool _initialized;

  ConcurrentQueue *queue;
};
#endif //SERVER_ENCODER_H
