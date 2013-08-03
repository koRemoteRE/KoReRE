#ifndef RENDERPASS_H_
#define RENDERPASS_H_

#include "KoRE/Passes/ShaderProgramPass.h"
#include "KoRE/SceneNode.h"
#include "KoRE/Components/Camera.h"

class RenderPass : public kore::ShaderProgramPass
{
public:
  RenderPass(kore::FrameBuffer* gBuffer, kore::FrameBuffer* smBuffer, std::vector<kore::SceneNode*> lightNodes, kore::Camera* cam);
  ~RenderPass(void);
};

#endif //RENDERPASS_H_