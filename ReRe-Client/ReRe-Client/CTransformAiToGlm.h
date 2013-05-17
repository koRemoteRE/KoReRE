//
//  CTransformAiToGlm.h
//  ReReClient
//
//  Created by Thomas Kipshagen on 17.05.13.
//
//

#ifndef ReReClient_CTransformAiToGlm_h
#define ReReClient_CTransformAiToGlm_h

#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class CTransformAiToGlm
{
public:
    static glm::vec3* TransformCol3P(aiColor3D aim_AssimpColor)
    {
        return new glm::vec3(aim_AssimpColor.r,
                             aim_AssimpColor.g,
                             aim_AssimpColor.b);
    };
    
    static glm::vec3* TransformVec3P(aiVector3D aim_AssimpVector)
    {
        return new glm::vec3(aim_AssimpVector.x,
                             aim_AssimpVector.y,
                             aim_AssimpVector.z);
    };
    
    static glm::mat4* TransformMat4P(aiMatrix4x4 aim_AssimpMatrix)
    {
        return new glm::mat4(aim_AssimpMatrix.a1, aim_AssimpMatrix.a2, aim_AssimpMatrix.a3, aim_AssimpMatrix.a4,
                             aim_AssimpMatrix.b1, aim_AssimpMatrix.b2, aim_AssimpMatrix.b3, aim_AssimpMatrix.b4,
                             aim_AssimpMatrix.c1, aim_AssimpMatrix.c2, aim_AssimpMatrix.c3, aim_AssimpMatrix.c4,
                             aim_AssimpMatrix.d1, aim_AssimpMatrix.d2, aim_AssimpMatrix.d3, aim_AssimpMatrix.d4);
    };
    
    static glm::mat4 TransformMat4(aiMatrix4x4 aim_AssimpMatrix)
    {
        return glm::mat4(aim_AssimpMatrix.a1, aim_AssimpMatrix.a2, aim_AssimpMatrix.a3, aim_AssimpMatrix.a4,
                         aim_AssimpMatrix.b1, aim_AssimpMatrix.b2, aim_AssimpMatrix.b3, aim_AssimpMatrix.b4,
                         aim_AssimpMatrix.c1, aim_AssimpMatrix.c2, aim_AssimpMatrix.c3, aim_AssimpMatrix.c4,
                         aim_AssimpMatrix.d1, aim_AssimpMatrix.d2, aim_AssimpMatrix.d3, aim_AssimpMatrix.d4);
    };

};

#endif
