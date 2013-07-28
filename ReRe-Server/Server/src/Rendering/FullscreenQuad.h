#ifndef FULLSCREENQUAD_H_
#define FULLSCREENQUAD_H_

#include "KoRE/Common.h"
#include "KoRE/Mesh.h"

class FullscreenQuad : public kore::Mesh {
public:
  virtual ~FullscreenQuad();

  inline static FullscreenQuad* getInstance()
    {static FullscreenQuad instance; return &instance;}

private:
  FullscreenQuad();

  void init();

};

#endif  // FULLSCREENQUAD_H_