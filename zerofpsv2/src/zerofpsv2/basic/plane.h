#ifndef _BASIC_PLANE_H_
#define _BASIC_PLANE_H_

#include "vector3.h"
#include "basic_x.h"
//#include "os.h"

/**	\brief	Descibes a Plane in 3D.
		\ingroup Basic
*/
class BASIC_API Plane 
{
	public:
// Data
		Vector3	m_kNormal;								///< Normal of plane ( A,B,C in plane equation).
		float		m_fD;									///< Distance ( D in plane equation).

// Constructors
	Plane() { };
    Plane(const Plane& kPlane);
    Plane(const Vector3& kPoint0, const Vector3& kPoint1, const Vector3& kPoint2);

// Operators
	Plane& operator=( const Plane& kIn );
	
// Methods
		void Set(const float& fX, const float& fY, const float& fZ, const float& fInD) 
		{
			m_kNormal.Set(fX,fY,fZ);
			m_fD = fInD;
		}

		void Set(const Vector3& kNormal,const float& fInD) 
		{
			m_kNormal=kNormal;
			m_fD = fInD;
		}	
		
		void Set(const Vector3& kPoint0, const Vector3& kPoint1, const Vector3& kPoint2);
		void Set(const Vector3& kNormal, const Vector3& kPos);
	
		bool PointInside(const Vector3& kPoint);
		bool SphereInside(const Vector3& kCenter, const float& fRadius);
		
		float SphereTest(const Vector3& kPoint,const float& fRadius);
		float PointTest(const Vector3& kPoint);
		bool LineTest(const Vector3& kP1,const Vector3& kP2,Vector3* kColPos); 

		float SolveY(const float& X, const float& Z);

};

#endif






