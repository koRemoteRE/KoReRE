#ifndef SERVER_ENCODER_H
#define SERVER_ENCODER_H

#include "KoRE/Common.h"
#include <opencv2/core/core.hpp> 

class Encoder
{
public:
  Encoder(void);
  ~Encoder(void);
  std::vector<uchar>* encodeFrame();

private:
  glm::ivec2 _screenResolution;
  GLenum  _lastBuffer;
  std::vector<uchar> buff;
  cv::Mat _frame;
};
#endif //SERVER_ENCODER_H

