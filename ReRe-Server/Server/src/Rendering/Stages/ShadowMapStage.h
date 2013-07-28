#ifndef SHADOWMAPSTAGE_H_
#define SHADOWMAPSTAGE_H_

#include "KoRE/Passes/FrameBufferStage.h"
#include "Kore/Components/Camera.h"
#include "Kore/SceneNode.h"
#include "Rendering/DeferredPass.h"

class ShadowMapStage : public kore::FrameBufferStage {
public:
  ShadowMapStage(kore::SceneNode* lightNode,
                 std::vector<kore::SceneNode*>& vRenderNodes,
                 int shadow_width, int shadow_height);
  virtual ~ShadowMapStage();
};


#endif //SHADOWMAPSTAGE_H_
