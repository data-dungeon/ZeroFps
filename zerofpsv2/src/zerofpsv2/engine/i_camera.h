#ifndef _ENGINE_I_CAMERA_H_
#define _ENGINE_I_CAMERA_H_

#include "../basic/vector3.h"
#include "engine_x.h"

/// Interface to Camera.
class ENGINE_API I_Camera 
{		
	public:

		I_Camera(Vector3 kPos,Vector3 kRot,float fFov,float fAspect,float fNear,float fFar);
		virtual ~I_Camera();

		virtual void SetViewPort(float iX,float iY,float iW,float iH) = 0;
		virtual void RenderView() = 0;	
};

#endif // _ENGINE_I_CAMERA_H_




