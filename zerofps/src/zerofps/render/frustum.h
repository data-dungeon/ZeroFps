#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_

#include "render_x.h"
#include "../ogl/zfpsgl.h"
#include "../basic/basic.pkg"

using namespace std;

class RENDER_API Frustum : public ZFObject
{
	private:
		Vector4 m_akFrustum[6];
	
	public:
		Frustum();
		void GetFrustum();
		bool PointInFrustum(Vector3 kPoint);
		bool SphereInFrustum(Vector4 kPoint);
		bool CubeInFrustum( float x, float y, float z, float sizex,float sizey,float sizez );
};


#endif



