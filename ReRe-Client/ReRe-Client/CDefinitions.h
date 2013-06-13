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

#define PI 3.1454

// Define indices of the generic vertex attributes
#define SHADER_LIGHT_POS_LOC 0
#define SHADER_POSITION_LOC 1
#define SHADER_NORMAL_LOC 2
#define SHADER_TEX_COORD_LOC 3

#ifdef __APPLE_CC__
#define PATH "../Assets/"
#else
#define PATH "./Assets/"
#endif

#endif
