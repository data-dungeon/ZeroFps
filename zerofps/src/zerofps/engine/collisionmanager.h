#ifndef _COLLISIONMANAGER_H_
#define _COLLISIONMANAGER_H_


#include "engine.pkg"
#include "objects.pkg"
#include <vector>
#include <iostream>
#include <list>
#include "collisionproperty.h"


using namespace std;

class ENGINE_API CollisionManager {
	private:
		list<CollisionProperty*> m_akColPropertys;
		
	public:
		void Add(Object* kObject);
		void Remove(Object* kObject);
		void Remove(CollisionProperty* kColProperty);
		
		void Update();


};

#endif






