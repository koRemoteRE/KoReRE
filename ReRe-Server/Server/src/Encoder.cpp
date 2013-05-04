#include "Encoder.h"

#include <iostream>

#include "KoRE\RenderManager.h"


Encoder::Encoder(void)
  :buffer(NULL),
  _initialized(false),
  _recording(false), 
  frameno(0)
{
}


Encoder::~Encoder(void)
{
}

bool Encoder::init( char* filename, int width, int height )
{
  av_register_all();
  renderResolution = glm::ivec2(800,600);//kore::RenderManager::getInstance()->getRenderResolution();

  int framebuffersize = renderResolution.x*renderResolution.y*3;
  buffer = (uint8_t*)malloc(framebuffersize);

  codec = avcodec_find_encoder(CODEC_ID_MPEG1VIDEO);
  if (!codec){
    std::cerr << "codec not found" << std::endl;
    return false;
  }
  codecContext = avcodec_alloc_context3(codec);
  picRGB = avcodec_alloc_frame();
  picYUV = avcodec_alloc_frame();

  codecContext->bit_rate = 400000;
  codecContext->width = width;
  codecContext->height = height;
  AVRational timebase = {1,25};
  codecContext->time_base = timebase;
  codecContext->gop_size = 10; //keyframe every 10 frame
  codecContext->max_b_frames=1;
  codecContext->mb_decision=2;
  codecContext->pix_fmt = PIX_FMT_YUV420P;

  AVDictionary* conf = NULL;
  av_dict_set(&conf, "preset","ultrafast",0);
  if (avcodec_open2(codecContext, codec,&conf) < 0) {
    std::cerr << "could not open codec" << std::endl;
    return false;
  }
  file = fopen(filename, "wb");
  if (!file) {
    std::cerr << "could not open "<< filename << std::endl;
    return false;
  }

  outbuf_size = 100000;
  outbuf = (uint8_t *)malloc(outbuf_size);
  size =renderResolution.x*renderResolution.y;

  picRGB->data[0] = (uint8_t*)buffer+renderResolution.x*3*(renderResolution.y-1);
  picRGB->data[1] = picRGB->data[0] + size;
  picRGB->data[2] = picRGB->data[1] + size;
  picRGB->linesize[0] =-3*renderResolution.x;
  picRGB->linesize[1] =-3*renderResolution.x;
  picRGB->linesize[2] =-3*renderResolution.x;

  size = width*height;
  picYUVbuf = (uint8_t*)malloc((size * 3) / 2); // size for YUV 420 
  picYUV->data[0] = picYUVbuf;
  picYUV->data[1] = picYUV->data[0] + size;
  picYUV->data[2] = picYUV->data[1] + size / 4;
  picYUV->linesize[0] = width;
  picYUV->linesize[1] = width / 2;
  picYUV->linesize[2] = width / 2;

  sws_context = sws_getContext(renderResolution.x, renderResolution.y, PIX_FMT_BGR24,
                               width, height, PIX_FMT_YUV420P,
                               SWS_BICUBIC,NULL,NULL,NULL);
  if(sws_context == NULL) {
    std::cerr <<  "initialize of conversion context failed!" << std::endl;
    return false;
  }
  _initialized = true;
  frameno = 0;

  return true;
}

void Encoder::encodeFrame()
{
  if(!_recording) return;
  glPixelStorei(GL_PACK_ALIGNMENT,1);
  glPixelStorei(GL_PACK_ROW_LENGTH,0);
  glPixelStorei(GL_PACK_SKIP_ROWS,0);
  glPixelStorei(GL_PACK_SKIP_PIXELS,0);

  glGetIntegerv(GL_READ_BUFFER, (GLint*)&lastBuffer);
  glReadBuffer(GL_FRONT);
  glReadPixels(0,0, renderResolution.x,renderResolution.y,GL_BGR_EXT,GL_UNSIGNED_BYTE,buffer);
  glReadBuffer(lastBuffer);

  sws_scale(sws_context, picRGB->data,
            picRGB->linesize,0,
            renderResolution.y,
            picYUV->data,picYUV->linesize);

  out_size = avcodec_encode_video(codecContext,outbuf,outbuf_size,picYUV);
  std::cout << "encoding Frame " << ++frameno << "(size ="<<out_size <<")" << std::endl;
  fwrite(outbuf, 1, out_size, file);
}

void Encoder::finish()
{
  //delayed frames
  for(; out_size; ++frameno){
    out_size = avcodec_encode_video(codecContext, outbuf, outbuf_size, NULL);
    std::cout << "write frame "<<frameno<<" (size="<<out_size <<")"<<std::endl;
    fwrite(outbuf, 1, out_size, file); 
  }
  //add sequence end code to have a real mpeg file
  outbuf[0] = 0x00;
  outbuf[1] = 0x00;
  outbuf[2] = 0x01;
  outbuf[3] = 0xb7;
  fwrite(outbuf, 1, 4, file);
  fclose(file);
  free(buffer);
  free(picYUVbuf);
  free(outbuf);

  sws_freeContext(sws_context);
  avcodec_close(codecContext);
  av_free(codecContext);
  av_free(picRGB);
  av_free(picYUV);
  _recording=false;
}

void Encoder::start()
{
  if (_initialized){
    _recording = true;
  }
  else{
    std::cerr <<  "Encoder is not initialized!" << std::endl;
  }
}

void Encoder::stop()
{
  if (_recording){
    _recording = false;
  }
/*
  else{
    std::cerr <<  "Encoder is not recording!" << std::endl;
  }*/
}
