#ifndef _ZFPSGL_H_
#define _ZFPSGL_H_

#ifdef _WIN32
// Bloody Windows
	#include "extgl.h"

#else
// Sweet Linux.
	#define GL_GLEXT_PROTOTYPES   //experimental
	
	#include <GL/gl.h>
	#include <GL/glx.h>
	
#endif

#include "GL/glu.h"
//#include "GL/glut.h"


#define FOG_DISTANCE_MODE_NV 	0x855A
#define EYE_RADIAL_NV			0x855B
#define EYE_PLANE_ABSOLUTE_NV	0x855C

#endif







