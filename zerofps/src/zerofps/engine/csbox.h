#ifndef _CSBOX_H_
#define _CSBOX_H_

#include "collisionshape.h"
#include "../basic/basic.pkg"
#include "engine_x.h"
#include "cssphere.h"

using namespace std;

class ENGINE_API CSBox : public CollisionShape
{
	private:
		Vector3 m_kColPos;
		float coloffset;
		

	public:
		Vector3 m_kScale;
		
		CSBox(Vector3 kScale);	
		Collision* Test(CollisionShape* kOther,float fTime,bool bContinue);

		Collision* Collide_CSSphere(CSSphere* kOther,float fTime);
		
//		Vector3 TestQuad(Vector3 V1,Vector3 V2,Vector3 V3,Vector3 V4,Vector3 kPos1,Vector3 kPos2,float fR);

//		bool TestQuad(Vector3 kCenter,Vector3 kNormal,float d,float fWidth,float fHeight,Vector3 kPos1,Vector3 kPos2,float fR);

		bool TestTop(Vector3 kPos1,Vector3 kPos2,float fR);		
		bool TestBotom(Vector3 kPos1,Vector3 kPos2,float fR);				
		bool TestFront(Vector3 kPos1,Vector3 kPos2,float fR);		
		bool TestBack(Vector3 kPos1,Vector3 kPos2,float fR);						
		bool TestLeft(Vector3 kPos1,Vector3 kPos2,float fR);						
		bool TestRight(Vector3 kPos1,Vector3 kPos2,float fR);						
	
		bool TestInside(Vector3 kPos1,Vector3 kPos2,float fR);

		Vector3& Closest(Vector3& kCurPos,Vector3& OPos1,Vector3& OPos2);
		bool Closer(Vector3& kCurPos,Vector3& OPos1,Vector3& OPos2);
};


#endif





