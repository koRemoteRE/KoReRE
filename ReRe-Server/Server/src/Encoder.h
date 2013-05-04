#ifndef SERVER_ENCODER_H
#define SERVER_ENCODER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>


extern "C" { 
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/mathematics.h>
#include <libswscale/swscale.h>
}

class Encoder
{
public:
  Encoder(void);
  ~Encoder(void);
  bool init(char* Filename, int width, int height);
  void encodeFrame();
  void finish();
  void start();
  void stop();

private:
  uint8_t* buffer;
  glm::ivec2 renderResolution;
  GLenum 	lastBuffer;
  AVCodec* codec;
  AVCodecContext* codecContext;
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
  bool _recording;
  bool _initialized;
};
#endif //SERVER_ENCODER_H
