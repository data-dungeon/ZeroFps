#ifndef _COLOSIONOBJECT_H_
#define _COLOSIONOBJECT_H_

#include "engine_x.h"

enum COLLISION_TYPE {
	SPHERE_CO,
	BOX_CO,
	POINT_CO
};

class ENGINE_API CollisionObject {
	protected:


	public:
		int m_iType;
	
		virtual bool Collide(CollisionObject *kOther,bool bContinue)=0;
		
//		inline int &GetType(){ return m_iType ;};
};

#endif









