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

/**	\brief	A Frustum pyramid in 3D.
		\ingroup Render
*/
class RENDER_API Frustum 
{
	private:
		Vector4 		m_akFrustum[6];
		Plane			m_akFrustumPlanes[6];
		
	public:
		Frustum();
		void 	GetFrustum(const Matrix4& kProjectionMatrix,const Matrix4& kModelViewMatrix);
		
		bool 	PointInFrustum(const Vector3& kPoint);		
		bool 	SphereInFrustum(const Vector4& kPoint);			
		bool 	SphereInFrustum(const Vector3& kPos,float fRadius);				
		bool 	CubeInFrustum( float x, float y, float z, float sizex,float sizey,float sizez );
		bool 	CubeInFrustum ( const Vector3& kPos, const Vector3& kCenter, const Vector3& kSize, Matrix4 kRotation );

};


#endif



