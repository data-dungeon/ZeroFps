#ifndef _ENGINE_CSSPHERE_H_
#define _ENGINE_CSSPHERE_H_

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
		Collision* Test(CollisionShape* kOther,bool bContinue);

		Collision* Collide_CSSphere(CSSphere* kOther);
};


#endif







