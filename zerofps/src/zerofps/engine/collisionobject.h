#ifndef _COLOSIONOBJECT_H_
#define _COLOSIONOBJECT_H_

#include "engine_x.h"

class ENGINE_API CollisionObject {
	protected:


	public:
//		int m_iType;
	
		virtual bool Collide(CollisionObject *kOther,bool bContinue)=0;
};

#endif









