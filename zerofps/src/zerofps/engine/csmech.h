#ifndef _CSMECH_H_
#define _CSMECH_H_

#include "collisionshape.h"
#include "../basic/basic.pkg"
#include "engine_x.h"
#include "cssphere.h"

using namespace std;

class ENGINE_API CSMech : public CollisionShape
{
	private:
		Vector3 m_kColPos;
		Vector3 m_kColNormal;
		float m_fcoloffset;

	public:
		
		CSMech();	
		Collision* Test(CollisionShape* kOther,float fTime,bool bContinue);

		Collision* Collide_CSSphere(CSSphere* kOther,float fTime);
		
		Vector3& Closest(Vector3& kCurPos,Vector3& OPos1,Vector3& OPos2);
		bool Closer(Vector3& kCurPos,Vector3& OPos1,Vector3& OPos2);
		
		bool TestPolygon(Vector3* kVerts,Vector3 kPos1,Vector3 kPos2,float fR);
		bool TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos,float fR);
		
};


#endif

