#ifndef _BASIC_PLANE_H_
#define _BASIC_PLANE_H_

#include "vector3.h"
#include "basic_x.h"
#include "os.h"

/// Descibes a Plane in 3D.
class BASIC_API Plane 
{
	public:
		Vector3		m_kNormal;								///< Normal of plane ( A,B,C in plane equation).
		float		m_fD;									///< Distance ( D in plane equation).

		void Set(float fX, float fY, float fZ, float fInD) {
			m_kNormal.Set(fX,fY,fZ);
			m_fD = fInD;
		}
	
		void Set(Vector3 kNormal, float fInD) {
			m_kNormal=kNormal;
			m_fD = fInD;
		}	
	
		void Set(Vector3 kNormal, Vector3 kPos);
	
		bool PointInside(Vector3 kPoint);
		bool SphereInside(Vector3 kCenter, float fRadius);
		
		float SphereTest(Vector3 kPoint,float fRadius);
		float PointTest(Vector3 kPoint);
		bool LineTest(Vector3 kP1,Vector3 kP2,Vector3* kColPos); 

		float SolveY(float X, float Z);
		void Set(Vector3& kPoint0, Vector3& kPoint1, Vector3& Point2);

};

#endif






