#ifndef _COLLISIONPROPERTY_H_
#define _COLLISIONPROPERTY_H_

#include "property.h"
#include <iostream>

#include "objects.pkg"

using namespace std;

class ENGINE_API CollisionProperty:public Property{
	private:
		CollisionObject *m_pkColObject;

	public:
		CollisionProperty(Vector3 kPos,float iRadius);
		CollisionProperty(Vector3 kPos);
		CollisionProperty(Vector3 *kPos,float *iRadius);
		CollisionProperty(Vector3 *kPos);


		void Update();

};


#endif





