#ifndef _RENDER_FRUSTUM_H_
#define _RENDER_FRUSTUM_H_

#include "render_x.h"
#include "../ogl/zfpsgl.h"
#include "../basic/math.h"
#include "../basic/plane.h"
#include "../basic/zfsystem.h"

using namespace std;

/**	\brief	A Frustum pyramid in 3D.
		\ingroup Render
*/
class RENDER_API Frustum 
{
	private:
		Vector4 		m_akFrustum[6];
		Plane			m_akFrustumPlanes[6];
		
		int	m_iCubeCulls;
		int	m_iSphereCulls;
		int	m_iPointCulls;
		
	public:
		Frustum();
		
		int	GetCubeCulls()		{	int i = m_iCubeCulls;m_iCubeCulls=0; return i;		};
		int	GetSphereCulls()	{	int i = m_iSphereCulls;m_iSphereCulls=0; return i;	};
		int	GetPointCulls()	{	int i = m_iPointCulls;m_iPointCulls=0; return i;	};
		
		void 	GetFrustum(const Matrix4& kProjectionMatrix,const Matrix4& kModelViewMatrix);
		
		bool 	PointInFrustum(const Vector3& kPoint) const;		
		bool 	SphereInFrustum(const Vector4& kPoint) const;			
		bool 	SphereInFrustum(const Vector3& kPos,float fRadius) const;				
		bool 	CubeInFrustum( float x, float y, float z, float sizex,float sizey,float sizez ) const;
		bool 	CubeInFrustum( const Vector3& kPos, const Vector3& kCenter, const Vector3& kSize, Matrix4 kRotation ) const;
		bool	CubeInFrustum( const Vector3& kMin,const Vector3& kMax) const;
};


#endif



