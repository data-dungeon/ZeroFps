#ifndef _COLOSIONOBJECT_H_
#define _COLOSIONOBJECT_H_

//#include "basic.pkg"

enum COLLISION_TYPE {
	SPHERE_CO,
	BOX_CO,
	POINT_CO
};

class CollisionObject {
	protected:


	public:
		int m_iType;
	
		virtual bool Collide(CollisionObject *kOther,bool bContinue)=0;
		
//		inline int &GetType(){ return m_iType ;};
};

#endif









