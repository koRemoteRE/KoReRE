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
#define WIDTH 800
#define HEIGHT 600

#define CAM_POS_X 6.848972
#define CAM_POS_Y -5.918146
#define CAM_POS_Z 4.913852

#define CAM_LOOK_AT_X 0.0
#define CAM_LOOK_AT_Y 0.0
#define CAM_LOOK_AT_Z 0.5

#define CAM_UP_X 63.5593
#define CAM_UP_Y 0.6197659
#define CAM_UP_Z 46.69196

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
