#ifndef _CSSPHERE_H_
#define _CSSPHERE_H_

#include "collisionshape.h"
#include "../basic/basic.pkg"
#include "engine_x.h"
//#include <iostream>

//class CollisionShape;

using namespace std;

class ENGINE_API CSSphere : public CollisionShape
{
	private:

	public:
		float m_fRadius;
		
		CSSphere(float fRadius);	
		CollisionData* Test(CollisionShape* kOther,float fTime,bool bContinue);

		CollisionData* Collide_CSSphere(CSSphere* kOther,float fTime);
};


#endif







