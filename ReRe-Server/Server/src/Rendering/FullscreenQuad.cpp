#include "FullscreenQuad.h"

FullscreenQuad::FullscreenQuad() : kore::Mesh() {
  init();
}

FullscreenQuad::~FullscreenQuad() {
}

void FullscreenQuad::init() {
  _name = "_INTERNAL_FullscreenQuad";
  _numVertices = 4;
  _primitiveType = GL_TRIANGLES;

  const kore::uint LL = 0;
  const kore::uint LR = 1;
  const kore::uint UR = 2;
  const kore::uint UL = 3;

  // Load vertex positions.
  glm::vec3* vertices = new glm::vec3[_numVertices];
  vertices[LL] = glm::vec3(-1.0f, -1.0f, 0.0f);  // LL
  vertices[LR] = glm::vec3(1.0f, -1.0f, 0.0f);   // LR
  vertices[UR] = glm::vec3(1.0f, 1.0f, 0.0f);    // UR
  vertices[UL] = glm::vec3(-1.0f, 1.0f, 0.0f);   // UL
  
  kore::MeshAttributeArray att;
  att.name = "v_position";
  att.numValues = _numVertices * 3;
  att.numComponents = 3;
  att.type = GL_FLOAT_VEC3;
  att.componentType = GL_FLOAT;
  att.byteSize = kore::DatatypeUtil::getSizeFromGLdatatype(att.type);
  att.data = vertices;

  _attributes.push_back(att);


  // Load indices
  _indices.clear();
  _indices.resize(6);
  _indices[0] = LL;
  _indices[1] = LR;
  _indices[2] = UR;
  _indices[3] = UR;
  _indices[4] = UL;
  _indices[5] = LL;
  
  // Init the VBO and IBO
  createAttributeBuffers(kore::BUFFERTYPE_SEQUENTIAL);
}
