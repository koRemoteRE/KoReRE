#include "ShadowMapPass.h"
#include "KoRE/RenderManager.h"
#include "KoRE/SceneManager.h"
#include "KoRE/Operations/Operations.h"


ShadowMapPass::ShadowMapPass(std::vector<kore::SceneNode*>& vRenderNodes, kore::SceneNode* light, glm::uvec2 smSize)
{
  using namespace kore;

  RenderManager* renderMgr = RenderManager::getInstance();

  ShaderProgram* shader = new ShaderProgram;

  shader->loadShader("./assets/shader/ShadowMapVert.shader",
    GL_VERTEX_SHADER);

  shader->loadShader("./assets/shader/ShadowMapFrag.shader",
    GL_FRAGMENT_SHADER);

  shader->setName("shadow map shader");
  shader->init();
  
  this->setShaderProgram(shader);



  addStartupOperation(new EnableDisableOp(GL_DEPTH_TEST, EnableDisableOp::ENABLE));
  addStartupOperation(new ViewportOp(glm::ivec4(0,0,smSize.x, smSize.y)));
  //addStartupOperation(new ColorMaskOp(glm::bvec4(false, false, false, false)));
  addStartupOperation(new ClearOp());

  kore::Camera* lightcam = static_cast<Camera*>(light->getComponent(COMPONENT_CAMERA));

  for (uint i = 0; i < vRenderNodes.size(); ++i) {

    NodePass* nodePass = new NodePass(vRenderNodes[i]);
    this->addNodePass(nodePass);

    MeshComponent* meshComp =
      static_cast<MeshComponent*>(vRenderNodes[i]->getComponent(COMPONENT_MESH));

    nodePass
      ->addOperation(OperationFactory::create(OP_BINDATTRIBUTE, "v_position",
      meshComp, "v_position", shader));

    nodePass
      ->addOperation(new BindUniform(lightcam->getShaderData("view projection Matrix"),
      shader->getUniform("viewProjMat")));

    nodePass
      ->addOperation(OperationFactory::create(OP_BINDUNIFORM, "model Matrix",
      vRenderNodes[i]->getTransform(),
      "modelMat", shader));

    nodePass
      ->addOperation(new RenderMesh(meshComp));
  }
}


ShadowMapPass::~ShadowMapPass(void)
{
}
