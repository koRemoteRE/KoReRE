#ifndef DEFERREDPASS_H_
#define DEFERREDPASS_H_

#include "KoRE/Passes/ShaderProgramPass.h"
#include "KoRE/Components/Camera.h"
#include "KoRE/ShaderData.h"

class DeferredPass : public kore::ShaderProgramPass
{
public:
  DeferredPass(kore::Camera* cam, std::vector<kore::SceneNode*>& vRenderNodes);
  ~DeferredPass(void);
private:
  kore::ShaderData hasNM;
  kore::ShaderData hasNoNM;
  GLuint _hasNM;
  GLuint _hasNoNM;
};

#endif //DEFERREDPASS_H_