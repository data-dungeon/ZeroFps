#ifndef _BASIC_PLANE_H_
#define _BASIC_PLANE_H_

#include "vector3.h"
#include "basic_x.h"
#include "os.h"

/**	\brief	Descibes a Plane in 3D.
		\ingroup Basic
*/
class BASIC_API Plane 
{
	public:
// Data
		Vector3		m_kNormal;								///< Normal of plane ( A,B,C in plane equation).
		float		m_fD;									///< Distance ( D in plane equation).

// Constructors
	Plane() { }
    Plane(const Plane& kPlane);
    Plane(Vector3& kPoint0, Vector3& kPoint1, Vector3& kPoint2);

// Operators
	Plane& operator=( const Plane& kIn );
	
// Methods
		void Set(float fX, float fY, float fZ, float fInD) {
			m_kNormal.Set(fX,fY,fZ);
			m_fD = fInD;
		}

		void Set(Vector3 kNormal, float fInD) {
			m_kNormal=kNormal;
			m_fD = fInD;
		}	
		void Set(Vector3& kPoint0, Vector3& kPoint1, Vector3& kPoint2);
		void Set(Vector3 kNormal, Vector3 kPos);
	
		bool PointInside(Vector3 kPoint);
		bool SphereInside(Vector3 kCenter, float fRadius);
		
		float SphereTest(Vector3 kPoint,float fRadius);
		float PointTest(Vector3 kPoint);
		bool LineTest(Vector3 kP1,Vector3 kP2,Vector3* kColPos); 

		float SolveY(float X, float Z);

};

#endif






