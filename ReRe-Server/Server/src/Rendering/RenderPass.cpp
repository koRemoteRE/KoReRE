#include "RenderPass.h"
#include "FullscreenQuad.h"
#include "KoRE/Operations/Operations.h"
#include "KoRE/RenderManager.h"
#include "KoRE/SceneNode.h"
#include "Scene.h"


RenderPass::RenderPass(kore::FrameBuffer* gBuffer, kore::FrameBuffer* smBuffer,
                       std::vector<SceneNode*> lightNodes, kore::Camera* cam) {
  using namespace kore;

  RenderManager* renderMgr = RenderManager::getInstance();

  ShaderProgram* shader = new ShaderProgram;

  shader->loadShader("./assets/shader/finalRenderVert.shader",
                              GL_VERTEX_SHADER);

  shader->loadShader("./assets/shader/finalRenderFrag.shader",
                              GL_FRAGMENT_SHADER);
  shader->setName("final render shader");
  shader->init();
  

  kore::TexSamplerProperties texSamplerProps;
  texSamplerProps.wrapping = glm::uvec3(GL_CLAMP_TO_EDGE);
  texSamplerProps.minfilter = GL_NEAREST;
  texSamplerProps.magfilter = GL_NEAREST;

  shader->setSamplerProperties(0, texSamplerProps);
  shader->setSamplerProperties(1, texSamplerProps);
  shader->setSamplerProperties(2, texSamplerProps);
  shader->setSamplerProperties(3, texSamplerProps);
  shader->setSamplerProperties(4, texSamplerProps);
  this->setShaderProgram(shader);

  addStartupOperation(new EnableDisableOp(GL_DEPTH_TEST, EnableDisableOp::ENABLE));
  addStartupOperation(new ColorMaskOp(glm::bvec4(true, true, true, true)));
  addStartupOperation(new ViewportOp(glm::ivec4(0, 0,
                                     renderMgr->getScreenResolution().x,
                                     renderMgr->getScreenResolution().y)));
  addStartupOperation(new ClearOp());
  

  SceneNode* fsquadnode = new SceneNode();
  SceneManager::getInstance()->getRootNode()->addChild(fsquadnode);

  MeshComponent* fsqMeshComponent = new MeshComponent();
  fsqMeshComponent->setMesh(FullscreenQuad::getInstance());
  fsquadnode->addComponent(fsqMeshComponent);
  
  NodePass* nodePass = new NodePass(fsquadnode);
  this->addNodePass(nodePass);

  std::vector<ShaderData>& vGBufferTex = gBuffer->getOutputs();
  std::vector<ShaderData>& vSMBufferTex = smBuffer->getOutputs();
  
  nodePass->addOperation(new BindTexture(&vGBufferTex[0],
                         shader->getUniform("gBuffer_color")));
  nodePass->addOperation(new BindTexture(&vGBufferTex[1],
                         shader->getUniform("gBuffer_pos")));
  nodePass->addOperation(new BindTexture(&vGBufferTex[2],
                         shader->getUniform("gBuffer_normal")));
  nodePass->addOperation(new BindTexture(&vGBufferTex[3],
                        shader->getUniform("gBuffer_tangent")));
  nodePass->addOperation(new BindTexture(&vSMBufferTex[0],
                         shader->getUniform("shadowMap")));


  kore::Camera* lightcam = static_cast<Camera*>(lightNodes[0]->getComponent(COMPONENT_CAMERA));
  kore::LightComponent* light = static_cast<LightComponent*>(lightNodes[0]->getComponent(COMPONENT_LIGHT));
  
  nodePass
    ->addOperation(new BindUniform(lightcam->getShaderData("projection Matrix"),
    shader->getUniform("lightCamProjMat")));

  nodePass
    ->addOperation(new BindUniform(lightcam->getShaderData("view Matrix"),
    shader->getUniform("lightCamviewMat")));

  nodePass
    ->addOperation(new BindUniform(light->getShaderData("position"),
    shader->getUniform("lightPos")));
  nodePass
    ->addOperation(new BindUniform(light->getShaderData("direction"),
    shader->getUniform("lightDir")));

  nodePass
    ->addOperation(new BindUniform(cam->getShaderData("view Matrix"),
    shader->getUniform("viewMat")));

  nodePass
    ->addOperation(new BindUniform(cam->getShaderData("inverse view Matrix"),
    shader->getUniform("iViewMat")));

  nodePass->addOperation(OperationFactory::create(OP_BINDATTRIBUTE, 
                                                  "v_position",
                                                  fsqMeshComponent, 
                                                  "v_position",
                                                  shader));

  nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM, 
                                                  "ratio",
                                                  cam, 
                                                  "fRatio",
                                                  shader));

  nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM, 
                                                  "FOV degree",
                                                  cam, 
                                                  "fYfovDeg",
                                                  shader));

  nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM, 
                                                  "far Plane",
                                                  cam, 
                                                  "fFar",
                                                  shader));

  nodePass->addOperation(new BindUniform(renderMgr->getShdScreenRes(),
                                         shader->getUniform("screenRes"))); 


  nodePass->addOperation(OperationFactory::create(OP_BINDUNIFORM,
                                                  "inverse view Matrix",
                                                  cam,
                                                  "viewI",
                                                  shader));

  nodePass->addOperation(new RenderMesh(fsqMeshComponent));
}


RenderPass::~RenderPass(void)
{
}
