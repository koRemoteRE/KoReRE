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
  av_write_trailer(fc);
  // Close each codec. 
  if (videoStream){
    avcodec_close(videoStream->codec);
    //av_free(src_picture.data[0]);
    //av_free(dst_picture.data[0]);
    av_free(picRGB);
    av_free(picYUV);
  }
  if (!(fmt->flags & AVFMT_NOFILE))
    // Close the output file. 
      avio_close(fc->pb);

  // free the stream 
  avformat_free_context(fc);
}

AVStream *Encoder::add_stream(AVFormatContext *oc, AVCodec **codec,
                     enum AVCodecID codec_id)
{
  AVCodecContext *c;
  AVStream *st;

  // find the encoder
  *codec = avcodec_find_encoder(codec_id);
  if (!(*codec)) {
    fprintf(stderr, "Could not find encoder for '%s'\n",
      avcodec_get_name(codec_id));
    exit(1);
  }

  st = avformat_new_stream(oc, *codec);
  if (!st) {
    fprintf(stderr, "Could not allocate stream\n");
    exit(1);
  }
  st->id = oc->nb_streams-1;
  c = st->codec;

  c->codec_id = codec_id;

  c->bit_rate = 400000;
  // Resolution must be a multiple of two.
  c->width    = 800;
  c->height   = 600;
  // timebase: This is the fundamental unit of time (in seconds) in terms
  // of which frame timestamps are represented. For fixed-fps content,
  // timebase should be 1/framerate and timestamp increments should be
  // identical to 1. 
  c->time_base.den = 25;
  c->time_base.num = 1;
  c->gop_size      = 12; // emit one intra frame every twelve frames at most
  c->pix_fmt       = PIX_FMT_YUV420P;
  if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
    //just for testing, we also add B frames
    c->max_b_frames = 1;
  }
  if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
    // Needed to avoid using macroblocks in which some coeffs overflow.
    // This does not happen with normal video, it just happens here as
    // the motion of the chroma plane does not match the luma plane.
    c->mb_decision = 2;
    
  }

  // Some formats want stream headers to be separate.
  if (oc->oformat->flags & AVFMT_GLOBALHEADER)
    c->flags |= CODEC_FLAG_GLOBAL_HEADER;

  return st;
}



void Encoder::open_video(AVFormatContext *oc, AVCodec *codec, AVStream *st)
{
  int ret;
  AVCodecContext *c = st->codec;

  // open the codec 
  ret = avcodec_open2(c, codec, NULL);
  if (ret < 0) {
    fprintf(stderr, "Could not open video codec: %s\n");
    exit(1);
  }

  // allocate and init a re-usable frame 
  frame = avcodec_alloc_frame();
  if (!frame) {
    fprintf(stderr, "Could not allocate video frame\n");
    exit(1);
  }

  // Allocate the encoded raw picture. 
  ret = avpicture_alloc(&dst_picture, c->pix_fmt, c->width, c->height);
  if (ret < 0) {
    fprintf(stderr, "Could not allocate picture: %s\n");
    exit(1);
  }

  // If the output format is not YUV420P, then a temporary YUV420P
  // picture is needed too. It is then converted to the required
  // output format. 
  if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
    ret = avpicture_alloc(&src_picture, AV_PIX_FMT_YUV420P, c->width, c->height);
    if (ret < 0) {
      fprintf(stderr, "Could not allocate temporary picture: %s\n");
      exit(1);
    }
  }

  // copy data and linesize picture pointers to frame
  *((AVPicture *)frame) = dst_picture;
}


bool Encoder::init( char* filename, int width, int height )
{
  av_register_all();
  avformat_alloc_output_context2(&fc, NULL, NULL, filename);
  if (!fc) {
    printf("Could not deduce output format from file extension: using MPEG.\n");
    avformat_alloc_output_context2(&fc, NULL, "mp4", filename);
  }
  if (!fc) {
    exit(1);
  }
  fmt = fc->oformat;
  //fmt->video_codec = AV_CODEC_ID_H264;
  videoStream = NULL;
  if (fmt->video_codec != AV_CODEC_ID_NONE) {
    videoStream = add_stream(fc, &codec, fmt->video_codec);
  }
  if (videoStream){
    open_video(fc, codec, videoStream);
  }

  av_dump_format(fc, 0, filename,1);

  if (!(fmt->flags & AVFMT_NOFILE)) {
    ret = avio_open(&fc->pb, filename, AVIO_FLAG_WRITE);
    if (ret < 0) {
      fprintf(stderr, "Could not open '%s': %s\n", filename);
      exit(1);
    }
  }
  ret = avformat_write_header(fc, NULL);
  if (ret < 0) {
    fprintf(stderr, "Error occurred when opening output file: %s\n");
    exit(1);
  }
  picRGB = avcodec_alloc_frame();
  picYUV = avcodec_alloc_frame();
  if (picYUV)
    picYUV->pts = 0;






  renderResolution = glm::ivec2(800,600);//kore::RenderManager::getInstance()->getRenderResolution();

  int framebuffersize = renderResolution.x*renderResolution.y*3;
  buffer = (uint8_t*)malloc(framebuffersize);

  codecContext = videoStream->codec;
/*
  codec = avcodec_find_encoder(CODEC_ID_H264);
  if (!codec){
    std::cerr << "codec not found" << std::endl;
    return false;
  }
  codecContext = avcodec_alloc_context3(codec);


  codecContext->bit_rate = 400000;
  codecContext->width = width;
  codecContext->height = height;
  AVRational timebase = {1,25};
  codecContext->time_base = timebase;
  codecContext->gop_size = 10; //keyframe every 10 frame
  codecContext->max_b_frames=1;
  codecContext->mb_decision=2;
  codecContext->pix_fmt = PIX_FMT_YUV420P;
  codecContext->profile = FF_PROFILE_H264_BASELINE;

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
  }*/


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

  if (videoStream)
    video_pts = (double)videoStream->pts.val * videoStream->time_base.num /
    videoStream->time_base.den;
  else
    video_pts = 0.0;

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
  
  if (fc->oformat->flags & AVFMT_RAWPICTURE) {
    // Raw video case - directly store the picture in the packet 
    AVPacket pkt;
    av_init_packet(&pkt);

    pkt.flags        |= AV_PKT_FLAG_KEY;
    pkt.stream_index  = videoStream->index;
    pkt.data          = picYUV->data[0];
    pkt.size          = sizeof(AVFrame);

    ret = av_write_frame(fc, &pkt);
  } else {
    AVPacket pkt = { 0 };
    int got_packet;
    av_init_packet(&pkt);

    // encode the image 
    ret = avcodec_encode_video2(codecContext, &pkt, picYUV, &got_packet);
    if (ret < 0) {
      fprintf(stderr, "Error encoding video frame: %s\n");
      exit(1);
    }
    // If size is zero, it means the image was buffered. 

    if (!ret && got_packet && pkt.size) {
      pkt.stream_index = videoStream->index;

      // Write the compressed frame to the media file. 
        ret = av_write_frame(fc, &pkt);
    } else {
      ret = 0;
    }
  }
  if (ret != 0) {
    fprintf(stderr, "Error while writing video frame: %s\n");
    exit(1);
  }
  frame_count++;
  picYUV->pts += av_rescale_q(1, videoStream->codec->time_base, videoStream->time_base);
  //out_size = avcodec_encode_video(codecContext,outbuf,outbuf_size,picYUV);
  //std::cout << "encoding Frame " << ++frameno << "(size ="<<out_size <<")" << std::endl;
  //fwrite(outbuf, 1, out_size, file);
}

void Encoder::finish()
{

/*
  //delayed frames
  for(; out_size; ++frameno){
    out_size = avcodec_encode_video(codecContext, outbuf, outbuf_size, NULL);
    std::cout << "encoding frame "<<frameno<<" (size="<<out_size <<")"<<std::endl;
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
  av_free(codecContext);*/
  //av_free(picRGB);
  //av_free(picYUV);

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
