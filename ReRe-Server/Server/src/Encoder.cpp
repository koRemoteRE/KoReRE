
#include "Encoder.h"
#include "KoRE/Common.h"
#include "KoRE/RenderManager.h"
#include <opencv2/opencv.hpp>
#include "GL/glfw.h"





Encoder::Encoder(void){
  _screenResolution = kore::RenderManager::getInstance()->getScreenResolution();
  _frame.create(_screenResolution.y,_screenResolution.x,CV_8UC3);

  glGenBuffers(2, pboIds);
  glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[0]);
  glBufferData(GL_PIXEL_PACK_BUFFER, _screenResolution.y*_screenResolution.x*3, 0, GL_STREAM_READ);
  glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[1]);
  glBufferData(GL_PIXEL_PACK_BUFFER, _screenResolution.y*_screenResolution.x*3, 0, GL_STREAM_READ);

  glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}


Encoder::~Encoder(void){
}

std::vector<uchar>* Encoder::encodeFrame()
{
  double encodeTime = glfwGetTime();
  index = (index + 1) % 2;
  int nextIndex = (index + 1) % 2;
  glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[index]);
  glPixelStorei(GL_PACK_ALIGNMENT, (_frame.step & 3) ? 1 : 4);
  glPixelStorei(GL_PACK_ROW_LENGTH, _frame.step/_frame.elemSize());
  glGetIntegerv(GL_READ_BUFFER, (GLint*)&_lastBuffer);
  glReadBuffer(GL_FRONT);
  glReadPixels(0, 0, _frame.cols, _frame.rows, GL_BGR, GL_UNSIGNED_BYTE, 0);
  
  _frame.data = (uchar*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
  glReadBuffer(_lastBuffer); 
  std::cout << "pixel download time: " << glfwGetTime()-encodeTime << std::endl;
  //glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[nextIndex]);

  ////use fast 4-byte alignment (default anyway) if possible
  //glPixelStorei(GL_PACK_ALIGNMENT, (_frame.step & 3) ? 1 : 4);
  ////set length of one complete row in destination data (doesn't need to equal img.cols)
  //glPixelStorei(GL_PACK_ROW_LENGTH, _frame.step/_frame.elemSize());
  //
  //glGetIntegerv(GL_READ_BUFFER, (GLint*)&_lastBuffer);
  //glReadBuffer(GL_FRONT);
  //glReadPixels(0, 0, _frame.cols, _frame.rows, GL_BGR_EXT, GL_UNSIGNED_BYTE, _frame.data);
  //glReadBuffer(_lastBuffer); 
  //std::cout << "pixel download time: " << glfwGetTime()-encodeTime << std::endl;
  ////cv::flip(_frame, _frame, 0);
  
  //cv::Mat test = _frame;

  encodeTime = glfwGetTime();
  std::vector<int> param = std::vector<int>(2);
  //(2) png compression
  param[0]=CV_IMWRITE_JPEG_QUALITY;
  param[1]=90;//default(3)  0-9.
  cv::imencode(".jpg",_frame,buff,param);
  std::cout << "encode time: " << glfwGetTime()-encodeTime << std::endl;

  glUnmapBuffer(GL_PIXEL_PACK_BUFFER); 
  //std::cout<<"coded file size(png)"<<buff.size()<<std::endl;
  //cv::Mat pngimage = cv::imdecode(buff,CV_LOAD_IMAGE_COLOR);

  return &buff;
}