#include "Scene.h"
#include "KoRE/Common.h"
#include "KoRE/Operations/Operations.h"
#include "KoRE/SceneNode.h"
#include "KoRE/Passes/ShaderProgramPass.h"
#include "KoRE/Components/LightComponent.h"
#include "KoRE/GLerror.h"
#include "KoRE/Components/TexturesComponent.h"
#include "KoRE/ResourceManager.h"


Scene::Scene(void)
{
}


Scene::~Scene(void)
{
}

void Scene::setUpSimpleRendering(kore::SceneNode* renderNode, kore::ShaderProgramPass*
                          programPass, kore::Texture* texture, 
                          kore::LightComponent* light) {

        kore::NodePass* nodePass = new kore::NodePass;
        const kore::ShaderProgram* simpleShader = 
            programPass->getShaderProgram();
        kore::MeshComponent* pMeshComponent =
            static_cast<kore::MeshComponent*>
            (renderNode->getComponent(kore::COMPONENT_MESH));

        // Add Texture
        kore::GLerror::gl_ErrorCheckStart();
        kore::TexturesComponent* pTexComponent = new kore::TexturesComponent;
        pTexComponent->addTexture(texture);
        renderNode->addComponent(pTexComponent);
        kore::GLerror::gl_ErrorCheckFinish("Initialization");

              
        kore::BindAttribute* normAttBind =
            new kore::BindAttribute(pMeshComponent->getShaderData("v_normal"),
            simpleShader->getAttribute("v_normal"));

        kore::BindAttribute* uvAttBind =
            new kore::BindAttribute(pMeshComponent->getShaderData("v_uv0"),
            simpleShader->getAttribute("v_uv0"));

        // Bind Uniform-Ops
        kore::BindUniform* modelBind = 
            new kore::BindUniform(renderNode->getTransform()->
            getShaderData("model Matrix"), simpleShader->getUniform("model"));

        kore::BindUniform* viewBind =
            new kore::BindUniform(pCamera->getShaderData("view Matrix"),
            simpleShader->getUniform("view"));

        kore::BindUniform* projBind =
            new kore::BindUniform(pCamera->getShaderData("projection Matrix"),
            simpleShader->getUniform("projection"));

        kore::BindTexture* texBind =
            new kore::BindTexture(pTexComponent->
            getShaderData(texture->getName()),
            simpleShader->getUniform("tex"));

        kore::BindUniform* lightPosBind =
            new kore::BindUniform(light->getShaderData("position"),
            simpleShader->getUniform("pointlightPos"));

        kore::RenderMesh* pRenderOp = new kore::RenderMesh();
        pRenderOp->connect(pMeshComponent);

        nodePass->addOperation(kore::OperationFactory::create(kore::OP_BINDATTRIBUTE, "v_position", pMeshComponent, "v_position", simpleShader));
        nodePass->addOperation(normAttBind);
        nodePass->addOperation(uvAttBind);
        nodePass->addOperation(modelBind);
        nodePass->addOperation(viewBind);
        nodePass->addOperation(projBind);
        nodePass->addOperation(lightPosBind);
        nodePass->addOperation(texBind);
        nodePass->addOperation(pRenderOp);

        programPass->addNodePass(nodePass);
}

void Scene::setUpNMRendering(kore::SceneNode* renderNode, 
                      kore::ShaderProgramPass* programPass, 
                      kore::Texture* texture,
                      kore::Texture* normalmap,
                      kore::LightComponent* light) {

        kore::NodePass* nodePass = new kore::NodePass;
        const kore::ShaderProgram* nmShader = 
            programPass->getShaderProgram();
        kore::MeshComponent* pMeshComponent =
            static_cast<kore::MeshComponent*>
            (renderNode->getComponent(kore::COMPONENT_MESH));

        // Add Texture
        kore::GLerror::gl_ErrorCheckStart();
        kore::TexturesComponent* pTexComponent = new kore::TexturesComponent;
        pTexComponent->addTexture(texture);
        pTexComponent->addTexture(normalmap);
        renderNode->addComponent(pTexComponent);
        kore::GLerror::gl_ErrorCheckFinish("Initialization");

        // Bind Attribute-Ops
        kore::BindAttribute* posAttBind =
            new kore::BindAttribute(pMeshComponent->getShaderData("v_position"),
            nmShader->getAttribute("v_position"));

        kore::BindAttribute* normAttBind =
            new kore::BindAttribute(pMeshComponent->getShaderData("v_normal"),
            nmShader->getAttribute("v_normal"));

        kore::BindAttribute* tangentAttBind =
            new kore::BindAttribute(pMeshComponent->getShaderData("v_tangent"),
            nmShader->getAttribute("v_tangent"));

        kore::BindAttribute* uvAttBind =
            new kore::BindAttribute(pMeshComponent->getShaderData("v_uv0"),
            nmShader->getAttribute("v_uv0"));

        // Bind Uniform-Ops
        kore::BindUniform* modelBind = 
            new kore::BindUniform(renderNode->getTransform()->
            getShaderData("model Matrix"), nmShader->getUniform("model"));

        kore::BindUniform* normalMatBind = 
            new kore::BindUniform(renderNode->getTransform()->
            getShaderData("normal Matrix"), nmShader->getUniform("normal"));

        kore::BindUniform* viewBind =
            new kore::BindUniform(pCamera->getShaderData("view Matrix"),
            nmShader->getUniform("view"));

        kore::BindUniform* invViewBind =
          new kore::BindUniform(pCamera->getShaderData("inverse view Matrix"),
          nmShader->getUniform("viewI"));

        kore::BindUniform* projBind =
            new kore::BindUniform(pCamera->getShaderData("projection Matrix"),
            nmShader->getUniform("projection"));


        kore::BindTexture* texBind =
            new kore::BindTexture(pTexComponent->
            getShaderData(texture->getName()),
            nmShader->getUniform("tex"));

        kore::BindTexture* normalmapBind =
            new kore::BindTexture(pTexComponent->
            getShaderData(normalmap->getName()),
            nmShader->getUniform("normalmap"));

        kore::BindUniform* lightPosBind =
            new kore::BindUniform(light->getShaderData("position"),
            nmShader->getUniform("pointlightPos"));

        kore::RenderMesh* pRenderOp = new kore::RenderMesh();
        pRenderOp->connect(pMeshComponent);

        nodePass->addOperation(posAttBind);
        nodePass->addOperation(normAttBind);
        nodePass->addOperation(tangentAttBind);
        nodePass->addOperation(uvAttBind);
        nodePass->addOperation(modelBind);
        nodePass->addOperation(normalMatBind);
        nodePass->addOperation(viewBind);
        nodePass->addOperation(invViewBind);
        nodePass->addOperation(projBind);
        nodePass->addOperation(lightPosBind);
        nodePass->addOperation(texBind);
        nodePass->addOperation(normalmapBind);
        nodePass->addOperation(pRenderOp);

        programPass->addNodePass(nodePass);
}


void Scene::init()
{
  // enable culling and depthtest
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);


  // load shader
  kore::ShaderProgram* simpleShader = new kore::ShaderProgram;
  simpleShader->loadShader("./assets/shader/normalColor.vp",
    GL_VERTEX_SHADER);
  simpleShader->loadShader("./assets/shader/normalColor.fp",
    GL_FRAGMENT_SHADER);
  simpleShader->init();
  simpleShader->setName("cooler Shader");

  kore::ShaderProgram* nmShader = new kore::ShaderProgram;
  nmShader->loadShader("./assets/shader/normalmapping.vert", 
    GL_VERTEX_SHADER);
  nmShader->loadShader("./assets/shader/normalmapping.frag",
    GL_FRAGMENT_SHADER);
  nmShader->init();
  simpleShader->setName("normal mapping Shader");
  // load resources
  kore::ResourceManager::getInstance()
    ->loadScene("./assets/meshes/TestEnv.dae");
  //->loadScene("./assets/meshes/triangle.dae");

  // texture loading
  kore::Texture* testTexture =
    kore::ResourceManager::getInstance()->
    loadTexture("./assets/textures/Crate.png");

  kore::Texture* stoneTexture =
    kore::ResourceManager::getInstance()->
    loadTexture("./assets/textures/stonewall.png");

  kore::Texture* stoneNormalmap =
    kore::ResourceManager::getInstance()->
    loadTexture("./assets/textures/stonewall_NM_height.png");

  // find camera
  kore::SceneNode* pCameraNode = kore::SceneManager::getInstance()
    ->getSceneNodeByComponent(kore::COMPONENT_CAMERA);
  pCamera = static_cast<kore::Camera*>(
    pCameraNode->getComponent(kore::COMPONENT_CAMERA));

  // find light
  lightNode = kore::SceneManager::getInstance()
    ->getSceneNodeByComponent(kore::COMPONENT_LIGHT);
  kore::LightComponent* pLight = static_cast<kore::LightComponent*>(
    lightNode->getComponent(kore::COMPONENT_LIGHT));

  // select render nodes
  std::vector<kore::SceneNode*> vRenderNodes;
  kore::SceneManager::getInstance()->
    getSceneNodesByComponent(kore::COMPONENT_MESH, vRenderNodes);


  GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
  kore::FrameBufferStage* backBufferStage = new kore::FrameBufferStage;
  backBufferStage->setFrameBuffer(kore::FrameBuffer::BACKBUFFER);
  backBufferStage->setActiveAttachments(drawBuffers, 1);

  kore::ShaderProgramPass* shaderProgPass = new kore::ShaderProgramPass;
  //shaderProgPass->setShaderProgram(simpleShader);
  shaderProgPass->setShaderProgram(nmShader);

  // init operations
  for (kore::uint i = 0; i < vRenderNodes.size(); ++i) {

    //setUpSimpleRendering(vRenderNodes[i],shaderProgPass,testTexture,pLight);
    setUpNMRendering(vRenderNodes[i],shaderProgPass,stoneTexture,stoneNormalmap,pLight);

  }

  backBufferStage->addProgramPass(shaderProgPass);

  kore::RenderManager::getInstance()->addFramebufferStage(backBufferStage);

  std::vector<kore::SceneNode*> vBigCubeNodes;
  kore::SceneManager::getInstance()
    ->getSceneNodesByName("Cube", vBigCubeNodes);
  rotationNode = vBigCubeNodes[0]; 

  glClearColor(1.0f,1.0f,1.0f,1.0f);

}

void Scene::update(double time)
{
   kore::SceneManager::getInstance()->update();
   if (rotationNode) {
     rotationNode->rotate(90.0f * static_cast<float>(time), glm::vec3(0.0f, 0.0f, 1.0f));
   }
}
