#include "DeferredPass.h"
#include "KoRE/Passes/NodePass.h"
#include "KoRE/Operations/Operations.h"
#include "KoRE/Components/TexturesComponent.h"
#include "KoRE/RenderManager.h"
#include "KoRE/TextureSampler.h"


DeferredPass::DeferredPass(kore::Camera* cam, std::vector<kore::SceneNode*>& vRenderNodes){
  using namespace kore;
  
  kore::RenderManager* renderMgr = kore::RenderManager::getInstance();

  kore::ShaderProgram* shader = new kore::ShaderProgram;
  this->setShaderProgram(shader);

  shader->loadShader("./assets/shader/deferredVert.shader",
    GL_VERTEX_SHADER);
  shader->loadShader("./assets/shader/deferredFrag.shader",
    GL_FRAGMENT_SHADER);
  
  shader->setName("deferred shader");
  shader->init();

  addStartupOperation(new EnableDisableOp(GL_DEPTH_TEST, EnableDisableOp::ENABLE));
  addStartupOperation(new ColorMaskOp(glm::bvec4(true, true, true, true)));
  addStartupOperation(new ViewportOp(glm::ivec4(0, 0,
                                     renderMgr->getScreenResolution().x,
                                     renderMgr->getScreenResolution().y)));
  addStartupOperation(new ClearOp());

 
 
  _hasNM = 1;
  hasNM.data = &_hasNM;
  hasNM.type = GL_UNSIGNED_INT;

  _hasNoNM = 0;
  hasNoNM.data = &_hasNoNM;
  hasNoNM.type = GL_UNSIGNED_INT;
  
  for (uint i = 0; i < vRenderNodes.size(); ++i) {
    const TexturesComponent* texComp =
      static_cast<TexturesComponent*>(
      vRenderNodes[i]->getComponent(COMPONENT_TEXTURES));

    if (!texComp) {
      continue;
    }

    const Texture* diffusetex = texComp->getTexture(TEXSEMANTICS_DIFFUSE)[0];
    
    //const Texture* tex = texComp->getTexture(0);

    if (!diffusetex) {
      continue;
    }

    kore::TexSamplerProperties samplerProps;
    samplerProps.minfilter = GL_LINEAR_MIPMAP_LINEAR;
    shader->setSamplerProperties(0, samplerProps);

    NodePass* nodePass = new NodePass(vRenderNodes[i]);
    this->addNodePass(nodePass);

    MeshComponent* meshComp =
      static_cast<MeshComponent*>(vRenderNodes[i]->getComponent(COMPONENT_MESH));

    //Has Normalmap
    if (!texComp->getTexture(TEXSEMANTICS_NORMAL).empty()){
      const Texture* normaltex = texComp->getTexture(TEXSEMANTICS_NORMAL)[0];
      shader->setSamplerProperties(1, samplerProps);
      nodePass
        ->addOperation(new BindUniform(&hasNM,
        shader->getUniform("hasNM")));

      nodePass
        ->addOperation(OperationFactory::create(OP_BINDTEXTURE, normaltex->getName(),
        texComp, "normalTex", shader));
    }else{
     nodePass
        ->addOperation(new BindUniform(&hasNoNM,
        shader->getUniform("hasNM")));
    }

    nodePass
      ->addOperation(OperationFactory::create(OP_BINDATTRIBUTE, "v_position",
                                              meshComp, "v_position", shader));
    nodePass
      ->addOperation(OperationFactory::create(OP_BINDATTRIBUTE, "v_normal",
                                              meshComp, "v_normal", shader));
    nodePass
      ->addOperation(OperationFactory::create(OP_BINDATTRIBUTE, "v_tangent",
                                              meshComp, "v_tangent", shader));

    nodePass
      ->addOperation(OperationFactory::create(OP_BINDATTRIBUTE, "v_uv0",
                                              meshComp, "v_uv0", shader));
    nodePass
      ->addOperation(OperationFactory::create(OP_BINDUNIFORM, "projection Matrix",
                                              cam,"projectionMat", shader));
    nodePass
      ->addOperation(OperationFactory::create(OP_BINDUNIFORM, "view Matrix",
                                              cam, "viewMat", shader));
    nodePass
      ->addOperation(OperationFactory::create(OP_BINDUNIFORM, "model Matrix",
                                              vRenderNodes[i]->getTransform(),
                                              "modelMat", shader));
    nodePass
      ->addOperation(OperationFactory::create(OP_BINDUNIFORM, "normal Matrix",
                                              vRenderNodes[i]->getTransform(),
                                              "normalMat", shader));
    nodePass
      ->addOperation(OperationFactory::create(OP_BINDTEXTURE, diffusetex->getName(),
                                              texComp, "diffuseTex", shader));
    nodePass
      ->addOperation(new RenderMesh(meshComp));
  }
}


DeferredPass::~DeferredPass(void)
{
}
