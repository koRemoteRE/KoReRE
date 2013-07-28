#include "ShadowMapStage.h"
#include "Rendering/ShadowMapPass.h"

ShadowMapStage::ShadowMapStage(kore::SceneNode* lightNode,
                               std::vector<kore::SceneNode*>& vRenderNodes,
                               int shadow_width, int shadow_height) {
 
  kore::FrameBuffer* _shadowBuffer = new kore::FrameBuffer("shadowBuffer");
  std::vector<GLenum> drawBufs;
  drawBufs.push_back(GL_COLOR_ATTACHMENT0);
  this->setActiveAttachments(drawBufs);

  kore::STextureProperties SMprops;
  SMprops.width = shadow_width;
  SMprops.height = shadow_height;
  SMprops.targetType = GL_TEXTURE_2D;
  SMprops.format = GL_DEPTH_STENCIL;
  SMprops.internalFormat =  GL_DEPTH24_STENCIL8;
  SMprops.pixelType = GL_UNSIGNED_INT_24_8;
  _shadowBuffer->addTextureAttachment(SMprops,"ShadowMap",GL_DEPTH_STENCIL_ATTACHMENT);

  SMprops.format = GL_RGB;
  SMprops.internalFormat =  GL_RGB32F;
  SMprops.pixelType = GL_FLOAT;
  _shadowBuffer->addTextureAttachment(SMprops,"SMposition",GL_COLOR_ATTACHMENT0);

  this->setFrameBuffer(_shadowBuffer);
  this->addProgramPass(new ShadowMapPass(
                       vRenderNodes, lightNode,
                       glm::uvec2(SMprops.width, SMprops.height)));
}

ShadowMapStage::~ShadowMapStage() {

}

