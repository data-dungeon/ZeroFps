#ifndef _RENDER_FRUSTUM_H_
#define _RENDER_FRUSTUM_H_

#include "render_x.h"
#include "../ogl/zfpsgl.h"
#include "../basic/vector3.h"
#include "../basic/vector4.h"
#include "../basic/plane.h"
#include "../basic/zfsystem.h"
#include "../basic/matrix4.h"

using namespace std;

/// A Frustum pyramid in 3D.
class RENDER_API Frustum 
{
	private:
		Vector4 m_akFrustum[6];
		Plane	m_akFrustumPlanes[6];
		
	public:
		Frustum();
		void GetFrustum();
		
		bool PointInFrustum(Vector3 kPoint);		
		bool SphereInFrustum(Vector4 kPoint);
		bool SphereInFrustum(Vector4& kPoint);				
		bool SphereInFrustum(Vector3 kPos,float fRadius);				
		bool CubeInFrustum( float x, float y, float z, float sizex,float sizey,float sizez );
		bool CubeInFrustum ( Vector3 kPosition, Vector3 kSize, Matrix4 kRotation );
		
/*		bool StartUp()	{ return true;	}
		bool ShutDown()	{ return true;	}
		bool IsValid()	{ return true;	}
*/
};


#endif



