#ifndef SERVER_SCENE_H
#define SERVER_SCENE_H
#include "KoRE/Common.h"
#include "KoRE/Operations/Operations.h"
#include "KoRE/SceneNode.h"
#include "KoRE/Passes/ShaderProgramPass.h"
#include "KoRE/Components/LightComponent.h"
#include "KoRE/GLerror.h"
#include "KoRE/Components/TexturesComponent.h"
#include "KoRE/ResourceManager.h"
#include "KoRE/Passes/FrameBufferStage.h"


class Scene
{
public:
  Scene(void);
  ~Scene(void);
  void init();
  void update(double time);
  inline kore::Camera* getCam(){return pCamera;}
  
private:
  void setUpSimpleRendering(kore::SceneNode* renderNode, kore::ShaderProgramPass* programPass, kore::Texture* texture, kore::LightComponent* light);
  void setUpNMRendering(kore::SceneNode* renderNode, kore::ShaderProgramPass* programPass, kore::Texture* texture, kore::Texture* normalmap, kore::LightComponent* light);
  kore::SceneNode* rotationNode;
  kore::SceneNode* lightNode;
  kore::Camera* pCamera;
  kore::FrameBufferStage* _backbufferStage;

};
#endif //SERVER_SCENE_H
