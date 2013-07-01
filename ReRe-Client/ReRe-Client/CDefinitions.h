//
//  CDefinitions.h
//  ReReClient
//
//  Created by Martina Sekulla on 14.05.13.
//
//

#ifndef ReReClient_CDefinitions_h
#define ReReClient_CDefinitions_h

// Define Window
#define WIDTH 1024
#define HEIGHT 576

#define CAM_POS_X 0.0
#define CAM_POS_Y 15.0
#define CAM_POS_Z 25.0

#define CAM_LOOK_AT_X -2.0
#define CAM_LOOK_AT_Y 15.0
#define CAM_LOOK_AT_Z 25.0

#define CAM_UP_X 0.0
#define CAM_UP_Y 1.0
#define CAM_UP_Z 0.0

#define CAM_ROTATION_SPEED 0.05
#define CAM_MOVE_SPEED 15

// Define indices of the generic vertex attributes
#define SHADER_LIGHT_POS_LOC 0
#define SHADER_POSITION_LOC 1
#define SHADER_NORMAL_LOC 2
#define SHADER_TEX_COORD_LOC 3

#define TEXTURE_ACTIVITY 0

#ifdef __APPLE_CC__
#define PATH "../Assets/"
#else
#define PATH "./Assets/"
#endif

#endif
