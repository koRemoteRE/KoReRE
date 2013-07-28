#ifndef GBUFFERSTAGE_H_
#define GBUFFERSTAGE_H_

#include "KoRE/Passes/FrameBufferStage.h"
#include "Kore/Components/Camera.h"
#include "Kore/SceneNode.h"
#include "Rendering/DeferredPass.h"

class GBufferStage : public kore::FrameBufferStage {
public:
  GBufferStage(kore::Camera* mainCamera, std::vector<kore::SceneNode*>& vRenderNodes,
              int width, int height);
  virtual ~GBufferStage();
};


#endif //GBUFFERSTAGE_H_
