#ifndef _COLLISIONPROPERTY_H_
#define _COLLISIONPROPERTY_H_

#include "property.h"
#include <iostream>

#include "objects.pkg"
#include "collisionmanager.h"

using namespace std;

class ENGINE_API CollisionProperty:public Property{
	private:
		CollisionObject *m_pkColObject;	
		CollisionManager *m_pkCollisionMan;
		
	public:
		CollisionProperty(Vector3 kPos,float iRadius);
		CollisionProperty(Vector3 kPos);
		CollisionProperty(Vector3 *kPos,float *iRadius);
		CollisionProperty(Vector3 *kPos);
	
		~CollisionProperty();

		void Init();

		inline CollisionManager *GetColMan() {return m_pkCollisionMan;};	
		inline void SetColMan(CollisionManager* pkCollisionMan) {m_pkCollisionMan=pkCollisionMan;};			
		inline CollisionObject *GetColObject(){return m_pkColObject;};

		
		void Update();

};


#endif





