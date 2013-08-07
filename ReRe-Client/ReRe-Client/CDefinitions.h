//
//  CDefinitions.h
//  ReReClient
//

#ifndef ReReClient_CDefinitions_h
#define ReReClient_CDefinitions_h

// Define Window
#define WIDTH 1280
#define HEIGHT 720

// Define Camera
#define CAM_POS_X -7.5
#define CAM_POS_Y 1.6
#define CAM_POS_Z 25

#define CAM_LOOK_AT_X -6.5
#define CAM_LOOK_AT_Y 1.6
#define CAM_LOOK_AT_Z 2.7

#define CAM_UP_X 1
#define CAM_UP_Y 0
#define CAM_UP_Z 0

#define CAM_ROTATION_SPEED 0.025
#define CAM_MOVE_SPEED 2

// Define indices of the generic vertex attributes
#define SHADER_LIGHT_POS_LOC 0
#define SHADER_POSITION_LOC 1
#define SHADER_NORMAL_LOC 2
#define SHADER_TEX_COORD_LOC 3

#define TEXTURE_ACTIVITY 1

#define FRAME_COUNT 5

#ifdef __APPLE_CC__
#define MESH_PATH "../Assets/"
#define TEXTURE_PATH "../Assets/textures/"
#else
#define MESH_PATH "./Assets/"
#define TEXTURE_PATH "./Assets/textures/"
#endif

#endif
