#include "GBufferStage.h"
#include "../DeferredPass.h"

GBufferStage::GBufferStage(kore::Camera* mainCamera,
                           std::vector<kore::SceneNode*>& vRenderNodes,
                           int width, int height) {
  std::vector<GLenum> drawBufs;
  drawBufs.resize(4);
  drawBufs[0] = GL_COLOR_ATTACHMENT0;
  drawBufs[1] = GL_COLOR_ATTACHMENT1;
  drawBufs[2] = GL_COLOR_ATTACHMENT2;
  drawBufs[3] = GL_COLOR_ATTACHMENT3;
  this->setActiveAttachments(drawBufs);

  kore::FrameBuffer* gBuffer = new kore::FrameBuffer("gBuffer");
  this->setFrameBuffer(gBuffer);

  kore::STextureProperties props;
  props.width = width;
  props.height = height;
  props.targetType = GL_TEXTURE_2D;

  props.format = GL_RGB;
  props.internalFormat = GL_RGB8;
  props.pixelType = GL_UNSIGNED_BYTE;
  gBuffer->addTextureAttachment(props,"DiffuseColor",GL_COLOR_ATTACHMENT0);

  props.format = GL_RGB;
  props.internalFormat = GL_RGB32F;
  props.pixelType = GL_FLOAT;
  gBuffer->addTextureAttachment(props,"Position",GL_COLOR_ATTACHMENT1);

  props.format = GL_RGB;
  props.internalFormat = GL_RGB32F;
  props.pixelType = GL_FLOAT;
  gBuffer->addTextureAttachment(props,"Normal",GL_COLOR_ATTACHMENT2);

  props.format = GL_RGB;
  props.internalFormat = GL_RGB32F;
  props.pixelType = GL_FLOAT;
  gBuffer->addTextureAttachment(props, "Tangent", GL_COLOR_ATTACHMENT3);

  props.format = GL_DEPTH_STENCIL;
  props.internalFormat = GL_DEPTH24_STENCIL8;
  props.pixelType = GL_UNSIGNED_INT_24_8;
  gBuffer->addTextureAttachment(props, "Depth_Stencil", GL_DEPTH_STENCIL_ATTACHMENT);

  //kore::Camera* lightcam = static_cast<Camera*>(lightNodes[0]->getComponent(COMPONENT_CAMERA));
  this->addProgramPass(new DeferredPass(mainCamera, vRenderNodes));


}

GBufferStage::~GBufferStage()
{

}

