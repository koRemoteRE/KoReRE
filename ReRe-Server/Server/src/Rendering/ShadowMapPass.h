#ifndef SHADOWMAPPASS_H_
#define SHADOWMAPPASS_H_

#include "KoRE/Passes/ShaderProgramPass.h"


class ShadowMapPass : public kore::ShaderProgramPass
{
public:
  ShadowMapPass(std::vector<kore::SceneNode*>& vRenderNodes,kore::SceneNode* light, glm::uvec2 smSize);
  ~ShadowMapPass(void);
};
#endif //SHADOWMAPPASS_H_
