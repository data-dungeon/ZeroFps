#ifndef _ENGINE_CSSPHERE_H_
#define _ENGINE_CSSPHERE_H_


#include "../engine_systems_x.h"
#include "collisionshape.h"

using namespace std;

class ENGINE_SYSTEMS_API CSSphere : public CollisionShape
{
	private:

	public:
		float m_fRadius;
		
		CSSphere(float fRadius);	
		Collision* Test(CollisionShape* kOther,bool bContinue);

		Collision* Collide_CSSphere(CSSphere* kOther);
};


#endif







